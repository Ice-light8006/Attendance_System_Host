#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    centralWidget = new QWidget(this);
    rootLayout = new QHBoxLayout(centralWidget);
    panelQsw = new QStackedWidget(this);
    tcpClientPanel = new TcpClientPanel(this);
    deviceControl = new DeviceControl(this);
    controlPanel = new ControlPanel(this);
    uidPanel = new UIDPanel(this);
    logEdit = new QPlainTextEdit(this);
    leftLayout = new QVBoxLayout;
    rightLayout = new QVBoxLayout;
    databaseManager = new DatabaseManager(this);
    databaseManager->initDatabase();
    databasePanel = new DatabasePanel(this);
    informationDialog = new InformationDialog(databaseManager,this);
    logEdit->setReadOnly(true);
    logEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    logEdit->setFixedWidth(750);

    //在日志框的右键菜单添加清除按钮
    connect(logEdit, &QPlainTextEdit::customContextMenuRequested,
            this, [this](const QPoint &pos)
            {
                QMenu *menu = logEdit->createStandardContextMenu();

                menu->addSeparator();

                QAction *clearAction = menu->addAction("清空");

                connect(clearAction, &QAction::triggered,
                        this, [this]()
                        {
                            logEdit->clear();
                        });

                menu->exec(logEdit->mapToGlobal(pos));

                delete menu;
            });


    this->setCentralWidget(centralWidget);
    panelQsw->addWidget(tcpClientPanel);

    panelQsw->setFixedSize(200,100);
    controlPanel->setFixedWidth(200);
    leftLayout->addWidget(panelQsw);
    leftLayout->addWidget(controlPanel);

    rootLayout->addLayout(leftLayout);
    rootLayout->addLayout(rightLayout);



    QObject::connect(tcpClientPanel,&TcpClientPanel::connectRequested,this,&MainWindow::connectButtonPressed);
    QObject::connect(this,&MainWindow::connectRequested,deviceControl,&DeviceControl::connectToHost);
    QObject::connect(deviceControl,&DeviceControl::enrollMode,controlPanel,&ControlPanel::labelToEnroll);
    QObject::connect(deviceControl,&DeviceControl::idleMode,controlPanel,&ControlPanel::labelToIdle);
    QObject::connect(controlPanel,&ControlPanel::toggleModeRequested,deviceControl,&DeviceControl::toggleWorkMode);
    QObject::connect(deviceControl,&DeviceControl::updateFingerTable,controlPanel,&ControlPanel::updateFingerCntLabel);
    QObject::connect(deviceControl,&DeviceControl::updateFingerTable,databaseManager,&DatabaseManager::updateFingerTable);
    QObject::connect(databaseManager,&DatabaseManager::updateTableView,databasePanel,&DatabasePanel::updateTableView);
    QObject::connect(databasePanel,&DatabasePanel::deleteInfoByIndex,this,&MainWindow::deleteSelectedFinger);
    QObject::connect(deviceControl,&DeviceControl::setDisableToggleButton,controlPanel,&ControlPanel::setToggleButtonDisable);
    QObject::connect(deviceControl,&DeviceControl::deviceChecking,controlPanel,&ControlPanel::labelToChecking);
    QObject::connect(deviceControl,&DeviceControl::deviceOnline,controlPanel,&ControlPanel::labelToConnected);
    QObject::connect(deviceControl,&DeviceControl::deviceOffline,controlPanel,&ControlPanel::labelToDisconnected);
    QObject::connect(deviceControl,&DeviceControl::connectionMessage,this,[this](const QString &message){
        logEdit->appendPlainText(QTime::currentTime().toString("[hh:mm:ss] ") + message);
    });
    QObject::connect(deviceControl,&DeviceControl::verifySucceed,
                     informationDialog,&InformationDialog::showInformation);
    QObject::connect(controlPanel,&ControlPanel::openUIDPanelRequested,this,[this](){
        uidPanel->show();
        uidPanel->raise();
        uidPanel->activateWindow();
        deviceControl->requestUIDList();
    });
    QObject::connect(deviceControl,&DeviceControl::UIDListReceived,uidPanel,&UIDPanel::onUIDListReceived);
    QObject::connect(deviceControl,&DeviceControl::UIDInfoReceived,uidPanel,&UIDPanel::onUIDInfoReceived);
    QObject::connect(deviceControl,&DeviceControl::UIDResultReceived,uidPanel,&UIDPanel::onUIDResultReceived);
    QObject::connect(deviceControl,&DeviceControl::UIDDisconnected,uidPanel,&UIDPanel::onDisconnected);
    QObject::connect(uidPanel,&UIDPanel::UIDListRequested,deviceControl,&DeviceControl::requestUIDList);
    QObject::connect(uidPanel,&UIDPanel::UIDQueryRequested,deviceControl,&DeviceControl::requestUIDByIndex);
    QObject::connect(uidPanel,&UIDPanel::UIDInsertRequested,deviceControl,&DeviceControl::insertUIDByIndex);
    QObject::connect(uidPanel,&UIDPanel::UIDUpdateRequested,deviceControl,&DeviceControl::updateUIDByIndex);
    QObject::connect(uidPanel,&UIDPanel::UIDDeleteRequested,deviceControl,&DeviceControl::deleteUIDByIndex);


    QObject::connect(deviceControl, &DeviceControl::studentInfoRequested,
                     databaseManager, &DatabaseManager::sendStudentInfoByFinger);
    QObject::connect(databaseManager, &DatabaseManager::studentInfoReady,
                     deviceControl, &DeviceControl::sendStudentInfo);

    setupEnrollLogConnections();
    setupVerifyLogConnections();



    databasePanel->setFixedWidth(750);
    rightLayout->addWidget(logEdit);
    rightLayout->addWidget(databasePanel);
}



void MainWindow::setupEnrollLogConnections()
{
    // 如果 deviceControl 还没实例化，请确保在这之前已经 new 出来了
    if (!deviceControl || !logEdit) return;

    // 内部辅助 Lambda，统一负责往 logEdit 追加带时间戳的日志
    auto appendLog = [this](const QString &message) {
        QString timeStr = QTime::currentTime().toString("[hh:mm:ss] ");
        logEdit->appendPlainText(timeStr + message);

        // 可选：让滚动条自动滚动到最底部，保持最新日志可见
        QScrollBar *scrollbar = logEdit->verticalScrollBar();
        scrollbar->setValue(scrollbar->maximum());
    };

    // --- 第一次录入 ---
    connect(deviceControl, &DeviceControl::enrollFirstPressed, this, [=]() {
        appendLog("第一次按下手指");
    });
    connect(deviceControl, &DeviceControl::enrollFirstGetImg, this, [=]() {
        appendLog("第一次采集指纹图像");
    });
    connect(deviceControl, &DeviceControl::enrollFirstGetImgFailed, this, [=]() {
        appendLog("第一次采集指纹图像失败");
    });
    connect(deviceControl, &DeviceControl::enrollFirstGenChar, this, [=]() {
        appendLog("从第一次采集的指纹图像提取指纹特征");
    });
    connect(deviceControl, &DeviceControl::enrollFirstGenCharFailed, this, [=]() {
        appendLog("从第一次采集的指纹图像提取指纹特征失败");
    });

    // --- 松开手指 ---
    connect(deviceControl, &DeviceControl::enrollReleased, this, [=]() {
        appendLog("松开手指");
    });

    // --- 第二次录入 ---
    connect(deviceControl, &DeviceControl::enrollSecondPressed, this, [=]() {
        appendLog("第二次按下");
    });
    connect(deviceControl, &DeviceControl::enrollSecondGetImg, this, [=]() {
        appendLog("第二次采集指纹图像");
    });
    connect(deviceControl, &DeviceControl::enrollSecondGetImgFailed, this, [=]() {
        appendLog("第二次采集指纹图像失败");
    });
    connect(deviceControl, &DeviceControl::enrollSecondGenChar, this, [=]() {
        appendLog("从第二次采集的指纹图像提取指纹特征");
    });
    connect(deviceControl, &DeviceControl::enrollSecondGenCharFailed, this, [=]() {
        appendLog("从第二次采集的指纹图像提取指纹特征失败");
    });

    // --- 特征比对与合成 ---
    connect(deviceControl, &DeviceControl::enrollMatch, this, [=]() {
        appendLog("比较两次指纹特征是否来自同一个手指");
    });
    connect(deviceControl, &DeviceControl::enrollMatchFailed, this, [=]() {
        appendLog("比较两次指纹特征是否来自同一个手指失败");
    });
    connect(deviceControl, &DeviceControl::enrollRegModel, this, [=]() {
        appendLog("把两次特征合成为一个指纹模版");
    });
    connect(deviceControl, &DeviceControl::enrollRegModelFailed, this, [=]() {
        appendLog("把两次指纹特征合成为一个指纹模版失败");
    });

    // --- 模板存储与完成 ---
    connect(deviceControl, &DeviceControl::enrollStoreChar, this, [=]() {
        appendLog("把模版保存到模块的指定ID");
    });
    connect(deviceControl, &DeviceControl::enrollStoreCharFailed, this, [=]() {
        appendLog("把模版保存到模块的指定ID失败");
    });
    connect(deviceControl, &DeviceControl::enrollSucceed, this, [=]() {
        appendLog("指纹录入成功！");
    });
    connect(deviceControl,&DeviceControl::ReadCardUID,this,[=](const QByteArray &data){
        QString str = "识别到卡片：" + data.toHex(' ').toUpper();
        appendLog(str);
    });
}

void MainWindow::setupVerifyLogConnections()
{
    // 如果 deviceControl 还没实例化，请确保在这之前已经 new 出来了
    if (!deviceControl || !logEdit) return;

    // 内部辅助 Lambda，统一负责往 logEdit 追加带时间戳的日志
    auto appendLog = [this](const QString &message) {
        QString timeStr = QTime::currentTime().toString("[hh:mm:ss] ");
        logEdit->appendPlainText(timeStr + message);

        // 可选：让滚动条自动滚动到最底部，保持最新日志可见
        QScrollBar *scrollbar = logEdit->verticalScrollBar();
        scrollbar->setValue(scrollbar->maximum());
    };

    // --- 验证模式：采集 ---
    connect(deviceControl, &DeviceControl::verifyPressed, this, [=]() {
        appendLog("验证模式：按下手指");
    });
    connect(deviceControl, &DeviceControl::verifyGetImg, this, [=]() {
        appendLog("验证模式：采集指纹图像");
    });
    connect(deviceControl, &DeviceControl::verifyGetImgFailed, this, [=]() {
        appendLog("验证模式：采集指纹图像失败");
    });

    // --- 验证模式：特征提取 ---
    connect(deviceControl, &DeviceControl::verifyGenChar, this, [=]() {
        appendLog("验证模式：提取指纹特征");
    });
    connect(deviceControl, &DeviceControl::verifyGenCharFailed, this, [=]() {
        appendLog("验证模式：提取指纹特征失败");
    });

    // --- 验证模式：搜索与匹配 ---
    connect(deviceControl, &DeviceControl::verifySearch, this, [=]() {
        appendLog("验证模式：指纹搜索成功");
    });
    connect(deviceControl, &DeviceControl::verifySearchFailed, this, [=]() {
        appendLog("验证模式：指纹搜索失败（未找到匹配指纹）");
    });

    // --- 验证模式：成功与失败 ---
    connect(deviceControl, &DeviceControl::verifySucceed, this, [=]() {
        appendLog("验证模式：指纹验证成功！");
    });
    connect(deviceControl, &DeviceControl::verifyFailed, this, [=]() {
        appendLog("验证模式：指纹验证失败");
    });
}

void MainWindow::connectButtonPressed()
{
    //QMessageBox::warning(this,"警告","按钮按下了");
    emit connectRequested(tcpClientPanel->getAddress(),tcpClientPanel->getPort());

}

void MainWindow::deleteSelectedFinger(quint16 cnt)
{
    deviceControl->deleteFingerByIndex(cnt);
}

MainWindow::~MainWindow() = default;
