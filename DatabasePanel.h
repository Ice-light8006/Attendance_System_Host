#ifndef DATABASEPANEL_H
#define DATABASEPANEL_H

#include <QWidget>
#include <QObject>
#include <QSqlTableModel>
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QMenu>
#include <QSqlError>
#include <QSqlRecord>

class DatabasePanel:public QWidget
{
    Q_OBJECT
public:
    DatabasePanel(QWidget* parent = nullptr);

private:
    QVBoxLayout* layout;
    QTableView* tableView;
    QSqlTableModel* tableModel;
    quint16 getFingerIdByRow(int row);
public slots:
    void updateTableView();
private slots:
    void deleteSelectedRow();
    void showTableContextMenu(const QPoint &pos);
signals:
    void deleteInfoByIndex(quint16 cnt);
};

#endif // DATABASEPANEL_H
