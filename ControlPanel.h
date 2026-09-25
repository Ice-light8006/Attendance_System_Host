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
    void updateFingerCnt(const QByteArray &data);
private:
    QVBoxLayout* layout;
    QPushButton* toggleModeButton;
    QLabel* workModeLabel;
    QLabel* fingerCntLabel;
signals:
    void toggleModeRequested();
};

#endif // CONTROLPANEL_H
