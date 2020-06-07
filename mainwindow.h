#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "datastruct.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void onConnectSlots();
    QString folderParse( QString &path);
    bool onReadLine(const QString &filePath);
    void onParseStringLine(const QString &str);
    void parseCSVFileName(QString &file, QString &strLine);
    void parseRefValue(QString &ref, QString &strLine);
    void parseStrLineTime(QString &time, QString &strLine);
    void parseIDValue(QString &id, QString strLine);
    void processText(QString &strLine);
    void processBegin(QString &time, QString &file,QString &strLine);
    void processSendOrder(QString &time, QString  &ref, QString &strLine);
    void processEnd(QString &time, QString & file, QString &strLine);
    void processSendOrderAns(QString &ansTime, QString &ansRef,QString &ansID, QString &strLine);
    void processOrderPush(QString &OPtime, QString &OPID, bool &OPStatus, QString &strLine);
    void showText();

public slots:
    void onClickedViewBtn();

private:
    Ui::MainWindow *ui;
    QString m_filename;
    QString m_lastFolder;
    int m_state;

    QString m_sTime;
    QString m_eTime;
    QString m_ref;
    QString m_file;

    QString m_ansRef;
    QString m_ansID;
    QString m_ansSTime;

    QString m_OPTime;
    QString m_OPID;
    bool m_OPStatus;

    SendOrderCache m_sendOrderCache;
    SendOrderAnsCache m_sendOrderAnsCache;
    SendOrder m_SendOrder;
    SendOrderAns m_SendOrderAns;
    OrderPush m_OrderPush;

    int m_orderCount;
    qint64 m_orderMax;
    qint64 m_orderMin;
    qint64 m_diffSum;
    qint64 m_diffAvg;
    int m_scannerCount;
};
#endif // MAINWINDOW_H
