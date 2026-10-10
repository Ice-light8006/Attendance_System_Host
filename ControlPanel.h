#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QObject>
#include <QString>
#include <QByteArray>

class ControlPanel:public QWidget
{
    Q_OBJECT
public:
    ControlPanel(QWidget* parent = nullptr);
public slots:
    void labelToEnroll();
    void labelToIdle();
    void updateFingerCntLabel(const QByteArray &data);
    void setToggleButtonDisable();
    void setToggleButtonEnable();
    void labelToChecking();
    void labelToConnected();
    void labelToDisconnected();
private:
    QVBoxLayout* layout;
    QPushButton* toggleModeButton;
    QPushButton* UIDManageButton;
    QLabel* workModeLabel;
    QLabel* fingerCntLabel;
    QLabel* connectionLabel;
    bool isDeviceConnected = false;
signals:
    void toggleModeRequested();
    void openUIDPanelRequested();
};

#endif // CONTROLPANEL_H
