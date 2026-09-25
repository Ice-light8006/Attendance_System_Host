#include "ControlPanel.h"

ControlPanel::ControlPanel(QWidget *parent)
    :QWidget(parent)
{
    toggleModeButton = new QPushButton("切换工作模式",this);
    workModeLabel = new QLabel("当前工作模式",this);
    layout = new QVBoxLayout(this);
    fingerCntLabel = new QLabel("AS608内指纹数量：",this);
    layout->addWidget(toggleModeButton);
    layout->addWidget(workModeLabel);
    layout->addWidget(fingerCntLabel);
    QObject::connect(toggleModeButton,&QPushButton::pressed,this,&ControlPanel::toggleModeRequested);
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
    toggleModeButton->setEnabled(true);
}

