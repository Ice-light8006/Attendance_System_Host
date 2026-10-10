#include "ControlPanel.h"

ControlPanel::ControlPanel(QWidget *parent)
    :QWidget(parent)
{
    toggleModeButton = new QPushButton("切换工作模式",this);
    toggleModeButton->setDisabled(true);
    UIDManageButton = new QPushButton("管理 UID 白名单",this);
    workModeLabel = new QLabel("当前工作模式",this);
    layout = new QVBoxLayout(this);
    fingerCntLabel = new QLabel("AS608内指纹数量：",this);
    connectionLabel = new QLabel("下位机未连接",this);
    layout->addWidget(connectionLabel);
    layout->addWidget(toggleModeButton);
    layout->addWidget(UIDManageButton);
    layout->addWidget(workModeLabel);
    layout->addWidget(fingerCntLabel);
    QObject::connect(toggleModeButton,&QPushButton::pressed,this,&ControlPanel::toggleModeRequested);
    QObject::connect(UIDManageButton,&QPushButton::pressed,this,&ControlPanel::openUIDPanelRequested);
}

static quint16 countOne(const QByteArray &data)
{
    quint16 ans = 0;
    for(auto i:data)
    {
        uint8_t tmp = i;
        while(tmp)
        {
            if(tmp&1)
            {
                ans++;
            }
            tmp = tmp >> 1;
        }
    }
    return ans;
}

void ControlPanel::labelToEnroll()
{
    workModeLabel->setText("当前工作模式：录入模式");
    setToggleButtonEnable();
}

void ControlPanel::labelToIdle()
{
    workModeLabel->setText("当前工作模式：验证模式");
    setToggleButtonEnable();
}

void ControlPanel::updateFingerCntLabel(const QByteArray &data)
{
    quint16 cnt = countOne(data);
    QString txt = QString("AS608内指纹数量：%1").arg(cnt);
    fingerCntLabel->setText(txt);
}

void ControlPanel::setToggleButtonDisable()
{
    toggleModeButton->setDisabled(true);
}

void ControlPanel::setToggleButtonEnable()
{
    toggleModeButton->setEnabled(isDeviceConnected);
}

void ControlPanel::labelToChecking()
{
    isDeviceConnected = false;
    connectionLabel->setText("正在确认下位机...");
    toggleModeButton->setDisabled(true);
}

void ControlPanel::labelToConnected()
{
    isDeviceConnected = true;
    connectionLabel->setText("已连接到下位机");
    setToggleButtonEnable();
}

void ControlPanel::labelToDisconnected()
{
    isDeviceConnected = false;
    connectionLabel->setText("下位机未连接");
    toggleModeButton->setDisabled(true);
}

