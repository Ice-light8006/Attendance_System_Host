#include "DatabasePanel.h"

DatabasePanel::DatabasePanel(QWidget *parent)
    :QWidget(parent)
{
    tableModel = new QSqlTableModel(this);
    tableModel->setTable("information");

    //OnFieldChange，表示改完一个单元格立刻自动写入数据库
    tableModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    tableModel->select(); // 执行查询，把数据从 SQLite 捞到内存里

    //把表头换成中文（否则默认显示 id, name, age...）
    tableModel->setHeaderData(0, Qt::Horizontal, "学号");
    tableModel->setHeaderData(1, Qt::Horizontal, "姓名");
    tableModel->setHeaderData(2, Qt::Horizontal, "年龄");
    tableModel->setHeaderData(3, Qt::Horizontal, "性别");
    tableModel->setHeaderData(4, Qt::Horizontal, "班级");
    tableModel->setHeaderData(5, Qt::Horizontal, "邮箱");
    tableModel->setHeaderData(6, Qt::Horizontal, "指纹ID");

    //初始化视图
    tableView = new QTableView(this);
    tableView->setModel(tableModel); //核心：把模型塞给视图

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中时整行选中
    tableView->horizontalHeader()->setStretchLastSection(true);     // 最后一列自动拉伸填满
    tableView->setAlternatingRowColors(true);                       // 隔行变色

    //右键菜单
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(tableView, &QTableView::customContextMenuRequested,
            this, &DatabasePanel::showTableContextMenu);

    layout = new QVBoxLayout(this);
    layout->addWidget(tableView);
}

void DatabasePanel::showTableContextMenu(const QPoint &pos)
{
    QModelIndex index = tableView->indexAt(pos);

    if (!index.isValid())
        return;

    tableView->setCurrentIndex(index);

    QMenu menu(this);

    QAction *deleteAction = menu.addAction("删除");

    connect(deleteAction, &QAction::triggered,
            this, &DatabasePanel::deleteSelectedRow);

    menu.exec(tableView->viewport()->mapToGlobal(pos));
}

void DatabasePanel::deleteSelectedRow()
{
    QModelIndex index = tableView->currentIndex();



    if (!index.isValid())
        return;

    emit deleteInfoByIndex(getFingerIdByRow(static_cast<int>(index.row())));

    int row = index.row();

    if (!tableModel->removeRow(row))
    {
        return;
    }


    if (!tableModel->submitAll())
    {

        tableModel->revertAll();
        return;
    }


    tableModel->select();
}

quint16 DatabasePanel::getFingerIdByRow(int row)
{
    // 检查行号是否在有效范围内
    if (row < 0 || row >= tableModel->rowCount()) {
        return 0; // 返回0表示无效的 finger_id（通常有效ID从1开始）
    }

    // 获取该行对应的完整记录
    QSqlRecord record = tableModel->record(row);

    // 通过字段名 "finger_id" 提取数据并转换为 quint16
    return static_cast<quint16>(record.value("finger_id").toUInt());
}

void DatabasePanel::updateTableView()
{
    tableModel->select();
}
