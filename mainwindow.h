#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QStackedWidget>
#include <QObject>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTime>
#include <QScrollBar>
#include <QMenu>
#include "DatabaseManager.h"
#include "DatabasePanel.h"
#include "TcpClientPanel.h"
#include "DeviceControl.h"
#include "ControlPanel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void setupEnrollLogConnections();
    void setupVerifyLogConnections();

    ~MainWindow() override;

private:
    QHBoxLayout* rootLayout;
    QVBoxLayout* leftLayout;
    QVBoxLayout* rightLayout;
    QStackedWidget* panelQsw;
    TcpClientPanel* tcpClientPanel;
    QWidget* centralWidget;
    DeviceControl* deviceControl;
    ControlPanel* controlPanel;
    QPlainTextEdit* logEdit;
    DatabasePanel* databasePanel;
    DatabaseManager* databaseManager;

private slots:
    void connectButtonPressed();
    void deleteSelectedFinger(quint16 cnt);
signals:
    void connectRequested(const QString &address,quint16 port);
};
#endif // MAINWINDOW_H
