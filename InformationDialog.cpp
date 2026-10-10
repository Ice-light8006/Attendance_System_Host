#include "InformationDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDebug>

static QString informationText(const QVariant &value)
{
    if(value.isNull() || value.toString().trimmed().isEmpty())
    {
        return "未填写";
    }
    return value.toString();
}

InformationDialog::InformationDialog(DatabaseManager* databaseManager,QWidget* parent)
    :QDialog(parent),databaseManager(databaseManager)
{
    setWindowTitle("个人信息");
    setWindowModality(Qt::ApplicationModal);
    setMinimumWidth(380);

    QVBoxLayout* layout = new QVBoxLayout(this);
    statusLabel = new QLabel(this);
    layout->addWidget(statusLabel);

    QFormLayout* formLayout = new QFormLayout;
    idLabel = new QLabel(this);
    nameLabel = new QLabel(this);
    ageLabel = new QLabel(this);
    sexLabel = new QLabel(this);
    classLabel = new QLabel(this);
    emailLabel = new QLabel(this);
    fingerIdLabel = new QLabel(this);
    scoreLabel = new QLabel(this);
    emailLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    formLayout->addRow("学号：",idLabel);
    formLayout->addRow("姓名：",nameLabel);
    formLayout->addRow("年龄：",ageLabel);
    formLayout->addRow("性别：",sexLabel);
    formLayout->addRow("班级：",classLabel);
    formLayout->addRow("邮箱：",emailLabel);
    formLayout->addRow("指纹 ID：",fingerIdLabel);
    formLayout->addRow("匹配分数：",scoreLabel);
    layout->addLayout(formLayout);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close,this);
    layout->addWidget(buttons);
    connect(buttons,&QDialogButtonBox::rejected,this,&QDialog::reject);
    connect(this,&QDialog::finished,this,[this](){
        if(!pendingResults.isEmpty())
        {
            showInformation(pendingResults.dequeue());
        }
    });
}

void InformationDialog::showInformation(const QByteArray &data)
{
    if(data.size()!=4)
    {
        qWarning() << "指纹验证成功数据长度不正确:" << data.size();
        return;
    }
    if(isVisible())
    {
        pendingResults.enqueue(data);
        return;
    }

    quint16 score = (static_cast<quint8>(data[0]) << 8) |
                    static_cast<quint8>(data[1]);
    quint16 fingerId = (static_cast<quint8>(data[2]) << 8) |
                       static_cast<quint8>(data[3]);
    QVariantMap info = databaseManager->getInfoByFinger(fingerId);

    statusLabel->setText(info.isEmpty() ? "验证成功，未找到对应的个人信息" : "指纹验证成功");
    idLabel->setText(informationText(info.value("id")));
    nameLabel->setText(informationText(info.value("name")));
    ageLabel->setText(informationText(info.value("age")));
    sexLabel->setText(informationText(info.value("sex")));
    classLabel->setText(informationText(info.value("class")));
    emailLabel->setText(informationText(info.value("email")));
    fingerIdLabel->setText(QString::number(fingerId));
    scoreLabel->setText(QString::number(score));

    show();
    raise();
    activateWindow();
}
