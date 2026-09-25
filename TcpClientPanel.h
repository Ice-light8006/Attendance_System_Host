#ifndef TCPCLIENTPANEL_H
#define TCPCLIENTPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QIntValidator>

class TcpClientPanel:public QWidget
{
    Q_OBJECT
public:
    TcpClientPanel(QWidget* parent = nullptr);
    QString getAddress();
    quint16 getPort();
private:
    QPushButton* connectButton;
    QLineEdit* addressInputBox;
    QLineEdit* portInputBox;
    QVBoxLayout* layout;
    QHBoxLayout* portLayout;
    QHBoxLayout* addressLayout;
    QLabel* portLabel;
    QLabel* addressLabel;
signals:
    void connectRequested();
};

#endif // TCPCLIENTPANEL_H
