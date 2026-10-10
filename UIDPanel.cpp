#include "UIDPanel.h"
#include "UIDProtocol.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpressionValidator>

UIDPanel::UIDPanel(QWidget *parent)
    :QWidget(parent),currentQuery(-1),pendingIndex(-1),
      pendingCommand(0),syncing(true),waitingOperation(false),
      operationSent(false),hasUIDList(false)
{
    setWindowFlag(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("UID 白名单管理");
    resize(520,650);

    uidOccupied.resize(256);
    tableModel = new QStandardItemModel(256,2,this);
    tableModel->setHeaderData(0,Qt::Horizontal,"栅格索引");
    tableModel->setHeaderData(1,Qt::Horizontal,"UID");
    for(int i = 0;i<256;i++)
    {
        tableModel->setItem(i,0,new QStandardItem(QString::number(i)));
        tableModel->setItem(i,1,new QStandardItem);
        setUIDRow(i,"—",false);
    }

    tableView = new QTableView(this);
    tableView->setModel(tableModel);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->setAlternatingRowColors(true);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(tableView);

    QObject::connect(tableView,&QTableView::doubleClicked,
                     this,&UIDPanel::onTableDoubleClicked);
    QObject::connect(tableView,&QTableView::customContextMenuRequested,
                     this,&UIDPanel::showTableContextMenu);
}

void UIDPanel::setUIDRow(int row,const QString &uid,bool occupied)
{
    uidOccupied[row] = occupied;
    tableModel->item(row,1)->setText(uid);
    for(int column = 0;column<2;column++)
    {
        QStandardItem* item = tableModel->item(row,column);
        if(occupied)
        {
            //已占用行沿用指纹列表的主题颜色和隔行变色
            item->setData(QVariant(),Qt::BackgroundRole);
            item->setData(QVariant(),Qt::ForegroundRole);
        }
        else
        {
            item->setBackground(QColor(225,225,225));
            item->setForeground(QColor(65,65,65));
        }
    }
}

void UIDPanel::onUIDListReceived(const QByteArray &data)
{
    if(data.size()!=32)
    {
        QMessageBox::warning(this,"警告","收到的 UID 列表长度不正确");
        return;
    }

    pendingQueries.clear();
    currentQuery = -1;
    syncing = true;
    hasUIDList = true;
    for(int i = 0;i<256;i++)
    {
        bool occupied = (static_cast<quint8>(data[i/8]) >> (i%8)) & 1U;
        setUIDRow(i,occupied ? "读取中..." : "—",occupied);
        if(occupied)
        {
            pendingQueries.enqueue(i);
        }
    }
    requestNextUID();
}

void UIDPanel::requestNextUID()
{
    if(waitingOperation && !operationSent)
    {
        currentQuery = -1;
        sendPendingOperation();
        return;
    }
    if(pendingQueries.isEmpty())
    {
        currentQuery = -1;
        syncing = false;
        return;
    }
    currentQuery = pendingQueries.dequeue();
    emit UIDQueryRequested(static_cast<quint8>(currentQuery));
}

void UIDPanel::sendPendingOperation()
{
    if(!waitingOperation || operationSent)
    {
        return;
    }
    operationSent = true;
    quint8 index = static_cast<quint8>(pendingIndex);
    if(pendingCommand==UID_DELETE)
    {
        emit UIDDeleteRequested(index);
    }
    else if(pendingCommand==UID_UPDATE)
    {
        emit UIDUpdateRequested(index,pendingUID);
    }
    else
    {
        emit UIDInsertRequested(index,pendingUID);
    }
}

void UIDPanel::onUIDInfoReceived(quint8 index,const QByteArray &uid)
{
    if(currentQuery!=index || uid.size()!=4)
    {
        return;
    }
    setUIDRow(index,uid.toHex(' ').toUpper(),true);
    requestNextUID();
}

QString UIDPanel::resultText(quint8 result) const
{
    switch(result)
    {
    case UID_INDEX_ERROR: return "索引越界";
    case UID_EMPTY: return "该索引没有 UID";
    case UID_DUPLICATE: return "这个 UID 已存储在其他索引";
    case UID_FULL: return "UID 存储已满";
    case UID_NOT_INIT: return "下位机存储尚未初始化";
    case UID_FLASH_ERROR: return "Flash 操作失败";
    case UID_STATE_ERROR: return "Flash 页面状态异常";
    case UID_ARG_ERROR: return "命令参数错误";
    case UID_OCCUPIED: return "该索引已经有 UID";
    default: return QString("未知错误：0x%1").arg(result,2,16,QChar('0')).toUpper();
    }
}

void UIDPanel::onUIDResultReceived(quint8 cmd,quint8 index,quint8 result)
{
    if(cmd==UID_QUERY)
    {
        if(currentQuery==index)
        {
            setUIDRow(index,"读取失败",true);
            requestNextUID();
        }
        return;
    }
    if(cmd==UID_REQUEST_LIST)
    {
        QMessageBox::warning(this,"警告",resultText(result));
        syncing = false;
        return;
    }
    if(!waitingOperation || pendingCommand!=cmd || pendingIndex!=index)
    {
        return;
    }
    waitingOperation = false;
    operationSent = false;
    pendingIndex = -1;
    if(result==UID_SUCCESS)
    {
        //收到下位机成功回包后更新对应栅格，再读取实际列表
        if(cmd==UID_DELETE)
        {
            setUIDRow(index,"—",false);
        }
        else
        {
            setUIDRow(index,pendingUID.toHex(' ').toUpper(),true);
        }
        pendingUID.clear();
        pendingQueries.clear();
        currentQuery = -1;
        syncing = true;
        emit UIDListRequested();
    }
    else
    {
        pendingUID.clear();
        QMessageBox::warning(this,"操作失败",resultText(result));
        if(result==UID_FLASH_ERROR)
        {
            //迁移可能已经提交，重新读取下位机的真实状态
            syncing = true;
            emit UIDListRequested();
        }
        else if(syncing)
        {
            requestNextUID();
        }
    }
}

void UIDPanel::onDisconnected()
{
    pendingQueries.clear();
    currentQuery = -1;
    pendingIndex = -1;
    waitingOperation = false;
    operationSent = false;
    hasUIDList = false;
    pendingUID.clear();
    syncing = true;
    for(int i = 0;i<256;i++)
    {
        setUIDRow(i,"—",false);
    }
}

void UIDPanel::showTableContextMenu(const QPoint &pos)
{
    QModelIndex index = tableView->indexAt(pos);
    if(!index.isValid() || !hasUIDList || waitingOperation ||
        !uidOccupied[index.row()])
    {
        return;
    }
    tableView->setCurrentIndex(index);
    QMenu menu(this);
    QAction* deleteAction = menu.addAction("删除");
    QObject::connect(deleteAction,&QAction::triggered,
                     this,&UIDPanel::deleteSelectedUID);
    menu.exec(tableView->viewport()->mapToGlobal(pos));
}

void UIDPanel::deleteSelectedUID()
{
    QModelIndex index = tableView->currentIndex();
    if(!index.isValid() || !uidOccupied[index.row()] ||
        !hasUIDList || waitingOperation)
    {
        return;
    }
    pendingIndex = index.row();
    pendingCommand = UID_DELETE;
    waitingOperation = true;
    pendingUID.clear();
    if(currentQuery<0)
    {
        sendPendingOperation();
    }
}

void UIDPanel::onTableDoubleClicked(const QModelIndex &index)
{
    if(!index.isValid() || waitingOperation)
    {
        return;
    }
    if(!hasUIDList)
    {
        QMessageBox::information(this,"提示","正在等待下位机的 UID 列表");
        return;
    }
    int row = index.row();
    QDialog dialog(this);
    dialog.setWindowTitle(uidOccupied[row] ? "修改 UID" : "插入 UID");
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QHBoxLayout* inputLayout = new QHBoxLayout;
    QLineEdit* inputs[4];
    QStringList oldBytes = tableModel->item(row,1)->text().split(' ');
    for(int i = 0;i<4;i++)
    {
        inputs[i] = new QLineEdit(&dialog);
        inputs[i]->setMaxLength(2);
        inputs[i]->setMaximumWidth(45);
        inputs[i]->setPlaceholderText("00");
        inputs[i]->setValidator(new QRegularExpressionValidator(
            QRegularExpression("[0-9A-Fa-f]{0,2}"),inputs[i]));
        if(uidOccupied[row] && oldBytes.size()==4)
        {
            inputs[i]->setText(oldBytes[i]);
        }
        inputLayout->addWidget(inputs[i]);
        if(i<3)
        {
            inputLayout->addWidget(new QLabel(":",&dialog));
        }
    }
    layout->addLayout(inputLayout);
    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,&dialog);
    layout->addWidget(buttons);
    QByteArray uid;
    QObject::connect(buttons,&QDialogButtonBox::accepted,&dialog,[&](){
        for(QLineEdit* input:inputs)
        {
            if(input->text().size()!=2)
            {
                QMessageBox::warning(&dialog,"输入错误",
                                     "UID 必须是四个字节，每个输入框填写两位十六进制数字");
                dialog.reject();
                return;
            }
            uid.append(static_cast<char>(input->text().toUInt(nullptr,16)));
        }
        dialog.accept();
    });
    QObject::connect(buttons,&QDialogButtonBox::rejected,
                     &dialog,&QDialog::reject);
    if(dialog.exec()!=QDialog::Accepted)
    {
        return;
    }

    pendingIndex = row;
    pendingCommand = uidOccupied[row] ? UID_UPDATE : UID_INSERT;
    pendingUID = uid;
    waitingOperation = true;
    if(currentQuery<0)
    {
        sendPendingOperation();
    }
}
