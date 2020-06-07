#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include "common.h"
#include <QDateTime>
using namespace trace;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),m_state(0)
{
    ui->setupUi(this);
    setWindowTitle("TIX日志分析工具");
    onConnectSlots();
    m_orderMin = INT_MAX;
    m_orderMax = INT_MIN;
    m_orderCount = 0;
    m_diffAvg = 0;
    m_diffSum = 0;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectSlots()
{
    connect(this->ui->viewBtn,&QPushButton::clicked,this,&MainWindow::onClickedViewBtn);
}

QString MainWindow::folderParse(QString &path)
{
    int pos = path.lastIndexOf(QDir::separator(),-1);
    return path.left(pos);
}

bool MainWindow::onReadLine(const QString &filePath)
{
    QFile file(filePath);
    if(!file.exists()||(filePath.toLower().indexOf("tix") == -1))
    {
        QMessageBox::critical(this,"ERROR",QString("文件:[%1]不存在或文件名不包含tix").arg(filePath));
        return false ;
    }

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QMessageBox::critical(this,"ERROR",QString("read file:[%1]faild...").arg(filePath));
        return false;
    }
    while(!file.atEnd())
    {
        QByteArray line =file.readLine();
        QString strLine(line);
        processText(strLine);
        QCoreApplication::processEvents();
        QThread::usleep(500);

    }
    file.close();
    return true;
}

void MainWindow::onParseStringLine(const QString &str)
{
    QString strTmp(str);
    if(str.contains("begin")) {
        m_state = OnBegin;
        QString html=QString("<span style='color:green'> 第 %1 单</span>").arg(m_orderCount +1);
        this->ui->plainTextEdit->appendHtml(html);
        processBegin(m_sTime,m_file, strTmp);
    }
    else if(str.indexOf("SendOrderAns") > -1) {
        m_state = OnSendOrderAns;
        processSendOrderAns(m_ansSTime,m_ansRef,m_ansID,strTmp);
    }
    else if( str.indexOf("SendOrder") > -1) {
        m_state = OnSendOrder;
        processSendOrder(m_eTime,m_ref,strTmp);
    }
    else if(str.indexOf("end") > -1) {
        m_state = OnEnd;
        processEnd(m_eTime,m_file,strTmp);

    }
    else if(str.indexOf("OrderPush") > -1) {
        m_state = OnOrderPush;
        processOrderPush(m_OPTime, m_OPID, m_OPStatus,strTmp);
    }
    else {
        m_state = OnError;
    }
}

void MainWindow::parseCSVFileName(QString &file, QString &strLine)
{
    int pos_fs = strLine.indexOf("file:");
    int pos_fe = strLine.indexOf(".csv");
    file = strLine.mid(pos_fs + 5,pos_fe - pos_fs -1);
}

void MainWindow::parseRefValue(QString &ref, QString &strLine)
{
    int pos_s = strLine.indexOf("ref=");
    int pos_e = strLine.indexOf(",");
    ref = strLine.mid(pos_s + 4, pos_e - pos_s - 4);
}

void MainWindow::processText(QString &strLine)
{
    onParseStringLine(strLine);
    if(OnError == m_state) return;
    switch (m_state) {
    case OnBegin:
    {
        m_state = OnContiune;
        m_SendOrder.strTime = m_sTime;
        m_SendOrder.fileName = m_file;
        break;
    }
    case OnSendOrder:{
        m_state = OnContiune;
        //m_SendOrder.strETime = m_eTime;
        m_SendOrder.orderRef = m_ref;
        break;
    }
    case OnEnd:
    {
        //by ref
        QString str = QString("扫单开始时间: [%0],扫单完成时间:[%1] Ref:[%2], 下单文件:[%3]").arg(m_sTime).arg(m_eTime).arg(m_ref).arg(m_file);
        m_SendOrder.strETime = m_eTime;
        if(!m_SendOrder.orderRef.isEmpty())
        {
            m_sendOrderCache[m_SendOrder.orderRef] = m_SendOrder;
        }
        else
        {
            //drop
            m_sendOrderCache.clear();
        }
        this->ui->plainTextEdit->appendPlainText(str);
        this->ui->plainTextEdit->appendPlainText("");
        break;
    }
    case OnSendOrderAns:
    {
        //by id
        m_state = OnContiune;
        m_SendOrderAns.orderRef = m_ansRef;
        m_SendOrderAns.orderID = m_ansID ;
        m_SendOrderAns.strTime = m_ansSTime;
        m_sendOrderAnsCache[m_ansID] = m_SendOrderAns;
        break;
    }
    case OnOrderPush:
    {
        //by id
        m_OrderPush.orderID = m_OPID ;
        m_OrderPush.strTime = m_OPTime;
        m_SendOrderAns.strETime = m_OPTime;
        m_OrderPush.BStatus = m_OPStatus;
        m_SendOrderAns.BStatus = m_OPStatus;
        m_SendOrderAns.orderID = m_OPID;
        m_sendOrderAnsCache[m_OPID] = m_SendOrderAns;
        if(m_OPStatus) {
            if(m_sendOrderAnsCache.find(m_OrderPush.orderID)== m_sendOrderAnsCache.end())
            {
                //drop
                QString html=QString("<span style='color:red'>回单缓存中未找到订单ID:%1 </span>").arg(m_OrderPush.orderID);
                this->ui->plainTextEdit->appendHtml(html);
                return;
            }
            SendOrderAns SOA =  m_sendOrderAnsCache[m_OrderPush.orderID];

            if(m_sendOrderCache.find(SOA.orderRef) == m_sendOrderCache.end()) {
                m_state = OnError;
                QString html=QString("<span style='color:red'>下单缓存中未找到订单Ref:%1 </span>").arg(m_SendOrderAns.orderRef);
                this->ui->plainTextEdit->appendHtml(html);
                return;
            }
            SendOrder SO = m_sendOrderCache[SOA.orderRef];
            qDebug() <<"SO ref " <<SO.strTime  << "SOA "<< SOA.strETime;
            QDateTime start = QDateTime::fromString(SO.strTime, "yyyy-MM-dd hh:mm:ss.zzz");
            QDateTime end = QDateTime::fromString(SOA.strETime, "yyyy-MM-dd hh:mm:ss.zzz");
            qint64 s1 = start.toMSecsSinceEpoch();
            qint64 s2 = end.toMSecsSinceEpoch();
            qint64 diff = s2 - s1;
            QString str = QString("下单时间: [%1], 回单时间:[%2], 订单ID:[%3] 扫单毫秒时间:[%4] 回单毫秒时间:[%5] 耗时(毫秒):<span style='color:green'>[%6 ms]</span>")
                 .arg(SO.strTime).arg(SOA.strETime).arg(SOA.orderID).arg(s1).arg(s2).arg(diff);
            this->ui->plainTextEdit->appendHtml(str);
            m_orderMax =  std::max(m_orderMax,diff);
            m_orderMin = std::min(m_orderMin,diff);
            m_diffSum += diff;
            ++ m_orderCount;
            QString html=QString("<span style='color:red'>订单处理完成</span>");
            this->ui->plainTextEdit->appendHtml(html);
        }
        m_SendOrderAns.BStatus = m_OPStatus;
        m_sendOrderAnsCache[m_ansRef] = m_SendOrderAns;
        break;
    }
    default:
    {
        m_state = OnError;
        break;
    }
    }
}

void MainWindow::parseStrLineTime(QString &time, QString &strLine)
{
    int pos_t1 = strLine.indexOf('[');
    int pos_t2 = strLine.indexOf(']');
    time = strLine.mid(pos_t1+1,pos_t2 - pos_t1 - 1);
}

void MainWindow::parseIDValue(QString &id, QString strLine)
{
    int pos_s = strLine.indexOf("id=");
    strLine = strLine.mid(pos_s + 3);
    int pos_e = strLine.lastIndexOf(",");
    id = strLine.mid(0,pos_e - 1 );
}

void MainWindow::processBegin(QString &time, QString &file, QString &strLine)
{
    parseStrLineTime(time,strLine);
    parseCSVFileName(file, strLine);
}

void MainWindow::processSendOrder(QString &time, QString &ref, QString &strLine)
{
    parseStrLineTime(time,strLine);
    parseRefValue(ref,strLine);
}

void MainWindow::processEnd(QString &time, QString &file, QString &strLine)
{
    parseStrLineTime(time,strLine);
    parseCSVFileName(file, strLine);
}

void MainWindow::processSendOrderAns(QString &ansTime, QString &ansRef, QString &ansID, QString &strLine)
{
    parseStrLineTime(ansTime,strLine);
    parseRefValue(ansRef,strLine);
    parseIDValue(ansID,strLine);
}

void MainWindow::processOrderPush(QString &OPtime, QString &OPID, bool OPStatus, QString &strLine)
{
    parseStrLineTime(OPtime,strLine);
    parseIDValue(OPID, strLine);
    int pos = strLine.lastIndexOf("status=");
    QString key = strLine.mid(pos+7).trimmed();
    if(OrderDone.find(key) != OrderDone.end()) {
        OPStatus = OrderDone[key];
    }
    else if ( AlgoDone.find(key) !=AlgoDone.end() ) {
        OPStatus = AlgoDone[key];
    }
    else {
       //DROP
       OPStatus = false;
    }
}

void MainWindow::showText()
{
    this->ui->maxLine->setText(QString::number(m_orderMax));
    this->ui->minLine->setText(QString::number(m_orderMin));
    m_diffAvg = m_diffSum/m_orderCount;
    this->ui->avgLine->setText(QString::number(m_diffAvg));
}

#include<QDir>
void MainWindow::onClickedViewBtn()
{
    m_lastFolder = this->ui->logPathLine->text();
    folderParse(m_lastFolder);
    m_filename = QFileDialog::getOpenFileName(this,"open file",m_lastFolder);
    if(m_filename.isEmpty())
       QMessageBox::critical(this,"ERROR","open file failed...");
    this->ui->logPathLine->setText(m_filename);
    if(!onReadLine(m_filename)) return;
    showText();
}

