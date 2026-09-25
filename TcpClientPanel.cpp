#include "TcpClientPanel.h"

TcpClientPanel::TcpClientPanel(QWidget* parent)
    :QWidget(parent)
{
    layout = new QVBoxLayout(this);
    connectButton = new QPushButton("Connect",this);
    addressInputBox = new QLineEdit(this);
    portInputBox = new QLineEdit(this);
    portLabel = new QLabel("Port",this);
    addressLabel = new QLabel("Address",this);
    portLayout = new QHBoxLayout;
    addressLayout = new QHBoxLayout;

    //设置Port输入框只能输入数字
    QIntValidator *validator = new QIntValidator(this);
    portInputBox->setValidator(validator);

    addressLayout->addWidget(addressLabel);
    addressLayout->addWidget(addressInputBox);

    portLayout->addWidget(portLabel);
    portLayout->addWidget(portInputBox);

    layout->addLayout(addressLayout);
    layout->addLayout(portLayout);
    layout->addWidget(connectButton);

    QObject::connect(connectButton,&QPushButton::pressed,this,&TcpClientPanel::connectRequested);
}

QString TcpClientPanel::getAddress()
{
    return addressInputBox->text();
}

quint16 TcpClientPanel::getPort()
{
    return portInputBox->text().toUShort();
}
