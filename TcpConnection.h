#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QNetworkProxy>

class TcpConnection:public QObject
{
    Q_OBJECT
public:
    TcpConnection(QTcpSocket* socket,QObject* parent = nullptr);
    TcpConnection(QObject* parent = nullptr);
    void send(const QByteArray &data);
    void connectToHost(const QString &hostName, quint16 port);
    void disconnectFromHost();
    bool isConnected() const;

private:
    QTcpSocket* socket;
private slots:
    void receive();
signals:
    void connected();
    void disconnected();
    void received(const QByteArray &data);
    void errorOccurred(QAbstractSocket::SocketError error);
};

#endif // TCPCONNECTION_H
