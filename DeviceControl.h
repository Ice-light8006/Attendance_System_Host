#ifndef DEVICECONTROL_H
#define DEVICECONTROL_H

#include <QObject>
#include <QByteArray>
#include "TcpConnection.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "Protocol.h"

#define IDLE_MODE 0xA1
#define ENROLL_MODE 0xA2
#define UPDATA_FINGER_TABLE 0xA3

#define TOGGLE_MODE 0xB1    //通知下位机
#define DELETE_FINGER 0xB2



//录入指纹相关的宏定义
#define FIRST_PRESSED 0xC1         // 第一次按下手指

#define FIRST_GETIMG 0xC2          // 第一次采集指纹图像
#define FIRST_GETIMG_FAILED 0xD2   // 第一次采集指纹图像失败

#define FIRST_GENCHAR 0xC3         // 从第一次采集的指纹图像提取指纹特征
#define FIRST_GENCHAR_FAILED 0xD4  // 从第一次采集的指纹图像提取指纹特征失败

#define RELEASED 0xC4              // 松开手指

#define SECOND_PRESSED 0xC5        // 第二次按下

#define SECOND_GETIMG 0xC6         // 第二次采集指纹图像
#define SECOND_GETIMG_FAILED 0xD6  // 第二次采集指纹图像失败

#define SECOND_GENCHAR 0xC7        // 从第二次采集的指纹图像提取指纹特征
#define SECOND_GENCHAR_FAILED 0xD7 // 从第二次采集的指纹图像提取指纹特征失败

#define MATCH 0xC8                 // 比较两次指纹特征是否来自同一个手指
#define MATCH_FAILED 0xD8          // 比较两次指纹特征是否来自同一个手指失败

#define REG_MODEL 0xC9             // 把两次特征合成为一个指纹模版
#define REG_MODEL_FAILED 0xD9      // 把两次指纹特征合成为一个指纹模版失败

#define STORE_CHAR 0xCA            // 把模版保存到模块的指定ID
#define STORE_CHAR_FAILED 0xDA     // 把模版保存到模块的指定ID失败

#define ENROLL_SUCCEED 0xCB        // 指纹录入成功！


//验证指纹相关的宏定义
#define PRESSED 0xE1

#define GETIMG 0xE2
#define GETIMG_FAILED 0xE3

#define GENCHAR 0xE4
#define GENCHAR_FAILED 0xE5

#define SEARCH 0xE6
#define SEARCH_FAILED 0xE7

#define VERIFY_SUCCEED 0xE8
#define VERIFY_FAILED 0xE9

#define DELETE_FINGER_FAILED 0xA6	//通知上位机指纹删除失败
#define DELETE_FINGER_SUCCEED 0xA7	//通知上位机指纹删除成功


class DeviceControl:public QObject
{
    Q_OBJECT
public:
    DeviceControl(QObject* parent = nullptr);
    void parseCmd(quint8 cmd,const QByteArray &data);
    void parseEnrollCmd(quint8 cmd,const QByteArray &data);
    void parseVerifyCmd(quint8 cmd, const QByteArray &data);
    void deleteFingerByIndex(quint16 cnt);
public slots:
    void toggleWorkMode();
private:
    TcpClient* tcpClient;
    TcpServer* tcpServer;
    Protocol* protocol;
private slots:
    void onConnectedReceived(QList<QByteArray> &data);
signals:
    TcpConnection* connectToHost(const QString &address,quint16 port);
    void enrollMode(const QByteArray &data);
    void idleMode(const QByteArray &data);

    //录入模式的信号
    void enrollFirstPressed();
    void enrollFirstGetImg();
    void enrollFirstGetImgFailed();
    void enrollFirstGenChar();
    void enrollFirstGenCharFailed();
    void enrollReleased();
    void enrollSecondPressed();
    void enrollSecondGetImg();
    void enrollSecondGetImgFailed();
    void enrollSecondGenChar();
    void enrollSecondGenCharFailed();
    void enrollMatch();
    void enrollMatchFailed();
    void enrollRegModel();
    void enrollRegModelFailed();
    void enrollStoreChar();
    void enrollStoreCharFailed();
    void enrollSucceed();

    //验证模式的信号
    void verifyPressed();
    void verifyGetImg();
    void verifyGetImgFailed();
    void verifyGenChar();
    void verifyGenCharFailed();
    void verifySearch();
    void verifySearchFailed();
    void verifySucceed();
    void verifyFailed();

    //更新UI界面上的指纹数量信号
    void updateFingerTable(const QByteArray &data);

    void setDisableToggleButton();
};

#endif // DEVICECONTROL_H
