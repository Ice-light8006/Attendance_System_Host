#include "TcpConnection.h"

TcpConnection::TcpConnection(QTcpSocket* socket,QObject *parent)
    :QObject(parent),socket(socket)
{
    //强制绕过系统代理，解决局域网连接被拦截的问题
    socket->setProxy(QNetworkProxy::NoProxy);

    QObject::connect(socket,&QTcpSocket::disconnected,this,&TcpConnection::disconnected);
    QObject::connect(socket,&QTcpSocket::readyRead,this,&TcpConnection::receive);
    QObject::connect(socket,&QTcpSocket::errorOccurred,this,&TcpConnection::errorOccurred);
    QObject::connect(socket,&QTcpSocket::connected,this,&TcpConnection::connected);
}

TcpConnection::TcpConnection(QObject *parent)
{
    socket = new QTcpSocket(this);
    //强制绕过系统代理，解决局域网连接被拦截的问题
    socket->setProxy(QNetworkProxy::NoProxy);
    QObject::connect(socket,&QTcpSocket::disconnected,this,&TcpConnection::disconnected);
    QObject::connect(socket,&QTcpSocket::readyRead,this,&TcpConnection::receive);
    QObject::connect(socket,&QTcpSocket::errorOccurred,this,&TcpConnection::errorOccurred);
    QObject::connect(socket,&QTcpSocket::connected,this,&TcpConnection::connected);
}

void TcpConnection::send(const QByteArray &data)
{
    socket->write(data);
}

bool TcpConnection::isConnected() const
{
    return socket->state()==QAbstractSocket::ConnectedState;
}

void TcpConnection::connectToHost(const QString &hostName, quint16 port)
{
    this->socket->connectToHost(hostName,port);
}

void TcpConnection::disconnectFromHost()
{
    socket->abort();
}

void TcpConnection::receive()
{
    QByteArray tmp = socket->readAll();

    emit received(tmp);
}

QString TcpConnection::errorString() const
{
    return socket->errorString();
}
