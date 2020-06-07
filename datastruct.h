#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <QString>
#include <QVector>
#include <QMap>
class SendOrder {
public:
    QString strTime;
    QString orderRef;
    QString fileName;
    SendOrder(QString time,QString ref,QString name):strTime(time),orderRef(ref),fileName(name){}

};
class SendOrderAns {
public:
    QString strTime;
    QString orderRef;
    QString orderID;
    SendOrderAns(QString time, QString ref, QString id):strTime(time),orderRef(ref),orderID(id){}
};
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
