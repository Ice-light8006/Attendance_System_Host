#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "TcpConnection.h"
#include "Protocol.h"
#include <QObject>
#include <QVector>
#include <QByteArray>
#include <QMessageBox>

class TcpClient:public QObject
{
    Q_OBJECT
public:
    TcpClient(QObject* parent = nullptr);
    TcpConnection* connectToHost(const QString &address,quint16 port);
    void send(const QByteArray &data);
    void disconnectFromHost();
    bool isConnected() const;
private:
    TcpConnection* tcpConnection = nullptr;
    Protocol* protocol;
private slots:
    void onConnectionReceived(const QByteArray &data);
signals:
    void received(QList<QByteArray> &data);
    void connected();
    void disconnected();
};

#endif // TCPCLIENT_H
