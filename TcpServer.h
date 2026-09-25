#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "TcpConnection.h"
#include "Protocol.h"
#include <QObject>
#include <QVector>
#include <QTcpServer>
#include <QVector>
#include <QTcpSocket>

class TcpServer:public QObject
{
    Q_OBJECT
public:
    TcpServer(QObject* parent = nullptr);
private slots:
    void onNewConnection();
    void onDisconnected();
    void onConnectionReceived(QByteArray &data);
private:
    QVector<TcpConnection*> TcpList;
    QTcpServer* Server;
    Protocol* protocol;
signals:
    void received(QList<QByteArray> &data);
};

#endif // TCPSERVER_H
