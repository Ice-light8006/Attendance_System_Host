#ifndef UIDPANEL_H
#define UIDPANEL_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QQueue>
#include <QVector>
#include <QByteArray>

class UIDPanel:public QWidget
{
    Q_OBJECT
public:
    UIDPanel(QWidget* parent = nullptr);

public slots:
    void onUIDListReceived(const QByteArray &data);
    void onUIDInfoReceived(quint8 index,const QByteArray &uid);
    void onUIDResultReceived(quint8 cmd,quint8 index,quint8 result);
    void onDisconnected();

private slots:
    void showTableContextMenu(const QPoint &pos);
    void deleteSelectedUID();
    void onTableDoubleClicked(const QModelIndex &index);

private:
    QTableView* tableView;
    QStandardItemModel* tableModel;
    QVector<bool> uidOccupied;
    QQueue<int> pendingQueries;
    int currentQuery;
    int pendingIndex;
    quint8 pendingCommand;
    QByteArray pendingUID;
    bool syncing;
    bool waitingOperation;
    bool operationSent;
    bool hasUIDList;

    void setUIDRow(int row,const QString &uid,bool occupied);
    void requestNextUID();
    void sendPendingOperation();
    QString resultText(quint8 result) const;

signals:
    void UIDListRequested();
    void UIDQueryRequested(quint8 index);
    void UIDInsertRequested(quint8 index,const QByteArray &uid);
    void UIDUpdateRequested(quint8 index,const QByteArray &uid);
    void UIDDeleteRequested(quint8 index);
};

#endif // UIDPANEL_H
