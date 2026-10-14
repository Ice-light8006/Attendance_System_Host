#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QVariantMap>
#include <QVector>

#define SQL_NAME "information.db"

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);

    // 初始化数据库并建表
    bool initDatabase();

    bool addInformation(const quint32 &id,
                        const QString &name,
                        const quint8 &age,
                        const QString &sex,
                        const QString &schoolClass,
                        const QString &email,
                        const quint16 &finger_id
                        );

    bool addInformation(const quint16 &finger_id);

    QVariantMap getInfoByFinger(quint16 finger_id);

    bool isInfoExistsByFinger(quint16 finger_id);
public slots:
    // Function A: query, serialize and emit exactly one response.
    void sendStudentInfoByFinger(quint16 finger_id);
    void updateFingerTable(const QByteArray &data);

private:
    QSqlDatabase db;
signals:
    void studentInfoReady(const QByteArray &data);
    void updateTableView();
};

#endif // DATABASEMANAGER_H
