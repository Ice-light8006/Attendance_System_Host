#include "TcpClient.h"

TcpClient::TcpClient(QObject* parent)
    :QObject(parent)
{
    protocol = new Protocol(this);
}

TcpConnection* TcpClient::connectToHost(const QString &address, quint16 port)
{
    TcpConnection* connection = new TcpConnection(this);
    QObject::connect(connection,&TcpConnection::received,this,&TcpClient::onConnectionReceived);

    //监听真正的连接成功信号
    QObject::connect(connection, &TcpConnection::connected, this, [=](){
        qDebug() << "网络底层真正连接成功！地址:" << address << "端口:" << port;
        QMessageBox::information(nullptr, "提示", "设备连接成功！");
    });

    //监听连接失败信号（这样填错IP你马上就能知道）
    QObject::connect(connection, &TcpConnection::errorOccurred, this, [=](QAbstractSocket::SocketError err){
        qDebug() << "连接失败，错误码:" << err;
        QMessageBox::warning(nullptr, "警告", "无法连接到设备，请检查IP和端口！");
    });

    connection->connectToHost(address,port);
    this->tcpConnection = connection;
    return connection;
}

void TcpClient::send(const QByteArray &data)
{
    tcpConnection->send(data);
}

void TcpClient::onConnectionReceived(const QByteArray &data)
{
    if(data.isEmpty())
    {
        qWarning() << "TcpClient received empry!";
    }
    QList<QByteArray> tmp = protocol->Parser(data);

    emit received(tmp);
}