#include "TcpClient.h"

TcpClient::TcpClient(QObject* parent)
    :QObject(parent)
{
    protocol = new Protocol(this);
}

TcpConnection* TcpClient::connectToHost(const QString &address, quint16 port)
{
    protocol->reset();
    if(tcpConnection!=nullptr)
    {
        QObject::disconnect(tcpConnection,nullptr,this,nullptr);
        tcpConnection->disconnectFromHost();
        tcpConnection->deleteLater();
    }
    TcpConnection* connection = new TcpConnection(this);
    QObject::connect(connection,&TcpConnection::received,this,&TcpClient::onConnectionReceived);

    //监听真正的连接成功信号
    QObject::connect(connection, &TcpConnection::connected, this, [=](){
        if(tcpConnection!=connection) return;
        qDebug() << "网络底层真正连接成功！地址:" << address << "端口:" << port;
        emit connected();
    });
    QObject::connect(connection, &TcpConnection::disconnected, this, [=](){
        if(tcpConnection!=connection) return;
        emit disconnected();
    });

    //监听连接失败信号（这样填错IP你马上就能知道）
    QObject::connect(connection, &TcpConnection::errorOccurred, this, [=](QAbstractSocket::SocketError err){
        if(tcpConnection!=connection) return;
        emit connectionError(QString("TCP 错误：code=%1，%2")
                             .arg(static_cast<int>(err)).arg(connection->errorString()));
        qDebug() << "连接失败，错误码:" << err;
        QMessageBox::warning(nullptr, "警告", "无法连接到设备，请检查IP和端口！");
    });

    connection->connectToHost(address,port);
    this->tcpConnection = connection;
    return connection;
}

void TcpClient::send(const QByteArray &data)
{
    if(tcpConnection!=nullptr && tcpConnection->isConnected())
    {
        tcpConnection->send(data);
    }
}

bool TcpClient::isConnected() const
{
    return tcpConnection!=nullptr && tcpConnection->isConnected();
}

void TcpClient::disconnectFromHost()
{
    if(tcpConnection!=nullptr)
    {
        tcpConnection->disconnectFromHost();
    }
}

void TcpClient::onConnectionReceived(const QByteArray &data)
{
    if(data.isEmpty())
    {
        qWarning() << "TcpClient received empry!";
        return;
    }
    QList<QByteArray> tmp = protocol->Parser(data);

    emit received(tmp);
}
