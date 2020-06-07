#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <QString>
#include <QVector>
#include <QMap>
namespace trace {

enum OrderStatus {
    OnError =-1,
    OnBegin = 0,
    OnSendOrder,
    OnEnd,
    OnSendOrderAns,
    OnOrderPush,
    OnContiune = 100,
};
}
class SendOrder {
public:
    QString strTime;
    QString strETime;
    QString orderRef;
    QString fileName;
    SendOrder(QString time = "",QString eTime = "", QString ref = "", QString name =""):strTime(time),strETime(eTime),orderRef(ref),fileName(name){}
    void clear()
    {
        strTime.clear();
        orderRef.clear();
        fileName.clear();
        strETime.clear();
    }
};

class SendOrderAns {
public:
    QString strTime;
    QString orderRef;
    QString orderID;
    QString strETime;
    bool BStatus;
    SendOrderAns(QString time = "", QString eTime = "",QString ref = "", QString id = "",bool status = false):strTime(time),strETime(eTime), orderRef(ref),orderID(id),BStatus(status){}
    void clear() {
        strTime.clear();
        orderRef.clear();
        orderID.clear();
        strETime.clear();
    }
};
class OrderPush {
  public:
    QString strTime;
    QString orderID;
    bool BStatus;
    OrderPush(QString time ="", QString id = "", bool status = false):strTime(time),orderID(id),BStatus(status){}
    void clear(){
        strTime.clear();
        orderID.clear();
        BStatus = false;
    }
};

typedef QMap<QString,SendOrder> SendOrderCache;

typedef QMap<QString,SendOrderAns> SendOrderAnsCache;

const QMap<QString, bool> OrderDone={
    {"-1", false},
    {"0", false},
    {"1", false},
    {"2", false},
    {"3", false},
    {"4", false},
    {"5", true},
    {"6", true},
    {"7", false},
    {"8", true},
    {"9", true},
    {"40", true}
};
const QMap<QString, bool> AlgoDone ={
    {"49", false},
    {"48", false},
    {"50", true},
    {"52", true},
    {"54", false},
    {"56", true},
    {"57", false},
    {"65", false},
    {"67", true},
    {"69", false},
    {"6", false}
};

#endif // DATASTRUCT_H
