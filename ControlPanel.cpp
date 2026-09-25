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

void ControlPanel::labelToEnroll()
{
    workModeLabel->setText("当前工作模式：录入模式");
}

void ControlPanel::labelToIdle()
{
    workModeLabel->setText("当前工作模式：验证模式");
}

void ControlPanel::updateFingerCnt(const QByteArray &data)
{
    quint16 cnt = static_cast<quint16>(data[0]<<8) + static_cast<quint16>(data[1]);
    QString txt = QString("AS608内指纹数量：%1").arg(cnt);
    fingerCntLabel->setText(txt);
}
