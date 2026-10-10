#ifndef INFORMATIONDIALOG_H
#define INFORMATIONDIALOG_H

#include <QDialog>
#include <QByteArray>
#include <QLabel>
#include <QQueue>
#include "DatabaseManager.h"

class InformationDialog:public QDialog
{
    Q_OBJECT
public:
    InformationDialog(DatabaseManager* databaseManager,QWidget* parent = nullptr);

public slots:
    void showInformation(const QByteArray &data);

private:
    DatabaseManager* databaseManager;
    QLabel* statusLabel;
    QLabel* idLabel;
    QLabel* nameLabel;
    QLabel* ageLabel;
    QLabel* sexLabel;
    QLabel* classLabel;
    QLabel* emailLabel;
    QLabel* fingerIdLabel;
    QLabel* scoreLabel;
    QQueue<QByteArray> pendingResults;
};

#endif // INFORMATIONDIALOG_H
