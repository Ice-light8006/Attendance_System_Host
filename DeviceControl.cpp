#include "DeviceControl.h"

DeviceControl::DeviceControl(QObject* parent)
    :QObject(parent)
{
    tcpClient = new TcpClient(this);
    tcpServer = new TcpServer(this);
    protocol = new Protocol(this);
    heartbeatIntervalTimer = new QTimer(this);
    heartbeatTimeoutTimer = new QTimer(this);
    heartbeatIntervalTimer->setInterval(5000);
    heartbeatTimeoutTimer->setSingleShot(true);
    heartbeatTimeoutTimer->setInterval(3000);
    QObject::connect(tcpClient,&TcpClient::received,this,&DeviceControl::onConnectedReceived);
    QObject::connect(tcpServer,&TcpServer::received,this,&DeviceControl::onConnectedReceived);
    QObject::connect(this,&DeviceControl::connectToHost,this,&DeviceControl::prepareConnection);
    QObject::connect(this,&DeviceControl::connectToHost,tcpClient,&TcpClient::connectToHost);
    QObject::connect(tcpClient,&TcpClient::connected,this,&DeviceControl::onTcpConnected);
    QObject::connect(tcpClient,&TcpClient::disconnected,this,&DeviceControl::onTcpDisconnected);
    QObject::connect(tcpClient,&TcpClient::connectionError,this,&DeviceControl::connectionMessage);
    QObject::connect(heartbeatIntervalTimer,&QTimer::timeout,this,&DeviceControl::sendPing);
    QObject::connect(heartbeatTimeoutTimer,&QTimer::timeout,this,&DeviceControl::onHeartbeatTimeout);
}

void DeviceControl::parseCmd(quint8 cmd,const QByteArray &data)
{
    if(cmd>=0xA0 && cmd<=0xAF)
    {
        switch(cmd)
        {
        case ENROLL_MODE:
        {
            emit enrollMode(data);

            break;
        }
        case IDLE_MODE:
        {
            emit idleMode(data);
            break;
        }
        case UPDATA_FINGER_TABLE:
        {
            emit updateFingerTable(data);
            break;
        }
        case DELETE_FINGER_SUCCEED:
        {
            qDebug() << "下位机删除指纹成功";
            break;
        }
        case DELETE_FINGER_FAILED:
        {
            qDebug() << "下位机删除指纹失败";
            break;
        }
        case READ_CARD_SUCCEED:
        {
            emit ReadCardUID(data);
            break;
        }
        case UID_LIST:
        {
            if(data.size()==32)
            {
                emit UIDListReceived(data);
            }
            break;
        }
        case UID_RESULT:
        {
            if(data.size()<3)
            {
                break;
            }
            quint8 operation = static_cast<quint8>(data[0]);
            quint8 index = static_cast<quint8>(data[1]);
            quint8 result = static_cast<quint8>(data[2]);
            if(operation==UID_QUERY && result==UID_SUCCESS && data.size()==7)
            {
                emit UIDInfoReceived(index,data.mid(3,4));
            }
            else
            {
                emit UIDResultReceived(operation,index,result);
            }
            break;
        }
        case STUDENT_INFO_REQUEST:
        {
            if (data.size() != 2) break;
            const quint16 fingerId = (static_cast<quint8>(data[0]) << 8) |
                                     static_cast<quint8>(data[1]);
            emit connectionMessage(QString("Student request: finger=%1").arg(fingerId));
            emit studentInfoRequested(fingerId);
            break;
        }
        case HOST_PONG:
        {
            onPong(data);
            break;
        }
        }
    }
    else if(cmd>=0xC0&&cmd<=0xDF)
    {
        parseEnrollCmd(cmd,data);
    }
    else if(cmd>=0xE0&&cmd<0xEF)
    {
        parseVerifyCmd(cmd,data);
    }
    else
    {
        qWarning() << "Unknown command received:" << Qt::hex << cmd;
    }
}

void DeviceControl::parseEnrollCmd(quint8 cmd, const QByteArray &data)
{
    switch (cmd)
    {
    case FIRST_PRESSED:
    {
        emit enrollFirstPressed();
        break;
    }

    case FIRST_GETIMG:
    {
        emit enrollFirstGetImg();
        break;
    }

    case FIRST_GETIMG_FAILED:
    {
        emit enrollFirstGetImgFailed();
        break;
    }

    case FIRST_GENCHAR:
    {
        emit enrollFirstGenChar();
        break;
    }

    case FIRST_GENCHAR_FAILED:
    {
        emit enrollFirstGenCharFailed();
        break;
    }

    case RELEASED:
    {
        emit enrollReleased();
        break;
    }

    case SECOND_PRESSED:
    {
        emit enrollSecondPressed();
        break;
    }

    case SECOND_GETIMG:
    {
        emit enrollSecondGetImg();
        break;
    }

    case SECOND_GETIMG_FAILED:
    {
        emit enrollSecondGetImgFailed();
        break;
    }

    case SECOND_GENCHAR:
    {
        emit enrollSecondGenChar();
        break;
    }

    case SECOND_GENCHAR_FAILED:
    {
        emit enrollSecondGenCharFailed();
        break;
    }

    case MATCH:
    {
        emit enrollMatch();
        break;
    }

    case MATCH_FAILED:
    {
        emit enrollMatchFailed();
        break;
    }

    case REG_MODEL:
    {
        emit enrollRegModel();
        break;
    }

    case REG_MODEL_FAILED:
    {
        emit enrollRegModelFailed();
        break;
    }

    case STORE_CHAR:
    {
        emit enrollStoreChar();
        break;
    }

    case STORE_CHAR_FAILED:
    {
        emit enrollStoreCharFailed();
        break;
    }

    case ENROLL_SUCCEED:
    {
        emit enrollSucceed();
        break;
    }

    default:
    {
        break;
    }
    }
}

void DeviceControl::parseVerifyCmd(quint8 cmd, const QByteArray &data)
{
    switch (cmd)
    {
    case PRESSED:
    {
        emit verifyPressed();
        break;
    }

    case GETIMG:
    {
        emit verifyGetImg();
        break;
    }

    case GETIMG_FAILED:
    {
        emit verifyGetImgFailed();
        break;
    }

    case GENCHAR:
    {
        emit verifyGenChar();
        break;
    }

    case GENCHAR_FAILED:
    {
        emit verifyGenCharFailed();
        break;
    }

    case SEARCH:
    {
        emit verifySearch();
        break;
    }

    case SEARCH_FAILED:
    {
        emit verifySearchFailed();
        break;
    }

    case VERIFY_SUCCEED:
    {
        emit verifySucceed(data);
        break;
    }

    case VERIFY_FAILED:
    {
        emit verifyFailed();
        break;
    }

    default:
    {
        break;
    }
    }
}

void DeviceControl::deleteFingerByIndex(quint16 cnt)
{
    QByteArray tmp;
    tmp.resize(2);
    tmp[0] = static_cast<char>((cnt >> 8) & 0xFF);
    tmp[1] = static_cast<char>(cnt & 0xFF);
    QByteArray data = protocol->Builder(DELETE_FINGER,tmp);
    tcpClient->send(data);
}

void DeviceControl::requestUIDList()
{
    tcpClient->send(protocol->Builder(UID_REQUEST_LIST,""));
}

void DeviceControl::requestUIDByIndex(quint8 index)
{
    QByteArray data;
    data.append(static_cast<char>(index));
    tcpClient->send(protocol->Builder(UID_QUERY,data));
}

void DeviceControl::insertUIDByIndex(quint8 index,const QByteArray &uid)
{
    if(uid.size()!=4) return;
    QByteArray data;
    data.append(static_cast<char>(index));
    data.append(uid);
    tcpClient->send(protocol->Builder(UID_INSERT,data));
}

void DeviceControl::updateUIDByIndex(quint8 index,const QByteArray &uid)
{
    if(uid.size()!=4) return;
    QByteArray data;
    data.append(static_cast<char>(index));
    data.append(uid);
    tcpClient->send(protocol->Builder(UID_UPDATE,data));
}

void DeviceControl::deleteUIDByIndex(quint8 index)
{
    QByteArray data;
    data.append(static_cast<char>(index));
    tcpClient->send(protocol->Builder(UID_DELETE,data));
}

void DeviceControl::setLinkState(quint8 state)
{
    if(linkState==state)
    {
        return;
    }
    linkState = state;
    if(state==0)
    {
        emit deviceOffline();
    }
    else if(state==1)
    {
        emit deviceChecking();
    }
    else
    {
        emit deviceOnline();
    }
}

void DeviceControl::prepareConnection()
{
    heartbeatIntervalTimer->stop();
    heartbeatTimeoutTimer->stop();
    awaitingPong = false;
    missedPongs = 0;
    setLinkState(0);
    emit UIDDisconnected();
}

void DeviceControl::onTcpConnected()
{
    missedPongs = 0;
    awaitingPong = false;
    setLinkState(1);
    heartbeatIntervalTimer->start();
    sendPing();
}

void DeviceControl::onTcpDisconnected()
{
    qWarning() << "TCP 连接已断开";
    emit connectionMessage("TCP 连接已断开");
    heartbeatIntervalTimer->stop();
    heartbeatTimeoutTimer->stop();
    awaitingPong = false;
    missedPongs = 0;
    setLinkState(0);
    emit UIDDisconnected();
}

void DeviceControl::sendPing()
{
    if(!tcpClient->isConnected() || awaitingPong)
    {
        return;
    }
    pendingHeartbeatSequence = ++heartbeatSequence;
    QByteArray data;
    data.append(static_cast<char>(pendingHeartbeatSequence >> 8));
    data.append(static_cast<char>(pendingHeartbeatSequence & 0xFF));
    awaitingPong = true;
    tcpClient->send(protocol->Builder(HOST_PING,data));
    heartbeatTimeoutTimer->start();
}

void DeviceControl::onPong(const QByteArray &data)
{
    if(data.size()!=2 || !awaitingPong)
    {
        return;
    }
    quint16 sequence = (static_cast<quint8>(data[0]) << 8) |
                       static_cast<quint8>(data[1]);
    if(sequence!=pendingHeartbeatSequence)
    {
        return;
    }
    heartbeatTimeoutTimer->stop();
    awaitingPong = false;
    missedPongs = 0;
    if(linkState!=2)
    {
        setLinkState(2);
        requestUIDList();
    }
}

void DeviceControl::onHeartbeatTimeout()
{
    if(!awaitingPong)
    {
        return;
    }
    awaitingPong = false;
    missedPongs++;
    qWarning() << "下位机 PONG 超时，连续次数:" << missedPongs;
    emit connectionMessage(QString("下位机 PONG 超时（%1/3）").arg(missedPongs));
    if(missedPongs>=3)
    {
        qWarning() << "连续三次未收到 PONG，主动断开 TCP";
        emit connectionMessage("连续三次未收到 PONG，主动断开 TCP");
        heartbeatIntervalTimer->stop();
        setLinkState(0);
        tcpClient->disconnectFromHost();
    }
}



void DeviceControl::toggleWorkMode()
{
    QByteArray data =  protocol->Builder(TOGGLE_MODE,"");
    tcpClient->send(data);
    emit setDisableToggleButton();
}

void DeviceControl::onConnectedReceived(QList<QByteArray> &data)
{
    for(QByteArray i:data)
    {
        if(i.isEmpty())
        {
            qWarning()<<"收到空数据包";
            continue;
        }

        //先解析命令
        quint8 cmd = static_cast<quint8>(i[0]);
        i.remove(0,1);

        //解析好命令之后直接通过信号把命令和数据转发出去
        parseCmd(cmd,i);
    }
}

void DeviceControl::sendStudentInfo(const QByteArray &data)
{
    if (data.size() < 4 || data.size() > STUDENT_INFO_MAX_PAYLOAD ||
        static_cast<quint8>(data[0]) != STUDENT_INFO_VERSION)
        return;
    const quint16 fingerId = (static_cast<quint8>(data[2]) << 8) |
                             static_cast<quint8>(data[3]);
    if (!tcpClient->isConnected()) {
        emit connectionMessage(QString("Student response not sent: TCP disconnected, finger=%1").arg(fingerId));
        return;
    }
    emit connectionMessage(QString("Student response: finger=%1 status=%2 bytes=%3")
                           .arg(fingerId).arg(static_cast<quint8>(data[1])).arg(data.size()));
    tcpClient->send(protocol->Builder(STUDENT_INFO_RESPONSE, data));
}
