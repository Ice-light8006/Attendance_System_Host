#include "DeviceControl.h"

DeviceControl::DeviceControl(QObject* parent)
    :QObject(parent)
{
    tcpClient = new TcpClient(this);
    tcpServer = new TcpServer(this);
    protocol = new Protocol(this);
    QObject::connect(tcpClient,&TcpClient::received,this,&DeviceControl::onConnectedReceived);
    QObject::connect(tcpServer,&TcpServer::received,this,&DeviceControl::onConnectedReceived);
    QObject::connect(this,&DeviceControl::connectToHost,tcpClient,&TcpClient::connectToHost);
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
        emit verifySucceed();
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
