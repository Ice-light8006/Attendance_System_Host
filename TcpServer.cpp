#include "TcpServer.h"

TcpServer::TcpServer(QObject* parent)
    :QObject(parent)
{
    this->protocol = new Protocol(this);
    this->Server = new QTcpServer(this);
    QObject::connect(Server,&QTcpServer::newConnection,this,&TcpServer::onNewConnection);
}

void TcpServer::onNewConnection()
{
    QTcpSocket* socket = Server->nextPendingConnection();
    TcpConnection* connection = new TcpConnection(socket,this);
    this->TcpList.append(connection);

    QObject::connect(connection,&TcpConnection::disconnected,this,&TcpServer::onDisconnected);
}

void TcpServer::onDisconnected()
{
    TcpConnection* connection = qobject_cast<TcpConnection*>(sender());
    if(connection==nullptr)
    {
        return;
    }
    TcpList.removeOne(connection);

    connection->deleteLater();
}

void TcpServer::onConnectionReceived(QByteArray &data)
{
    if(data.isEmpty())
    {
        qWarning() << "TcpClient received empry!";
    }
    QList<QByteArray> tmp = protocol->Parser(data);

    emit received(tmp);
}