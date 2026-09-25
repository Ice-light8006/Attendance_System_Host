#include "DatabaseManager.h"

DatabaseManager::DatabaseManager(QObject *parent)
{

}

bool DatabaseManager::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(SQL_NAME);

    if(!db.open())
    {
        qWarning() << "数据库打开失败" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    bool success = query.exec("CREATE TABLE IF NOT EXISTS information("
                              "id INTEGER PRIMARY KEY,"
                              "name TEXT,"
                              "age INTEGER,"
                              "sex TEXT,"
                              "class TEXT,"
                              "email TEXT,"
                              "finger_id INTEGER UNIQUE"
                              ")");
    if(!success)
    {
        qWarning() << "建表失败" << query.lastError().text();
        return false;
    }

    return true;

}

bool DatabaseManager::addInformation(const quint32 &id, const QString &name, const quint8 &age, const QString &sex, const QString &schoolClass, const QString &email, const quint16 &finger_id)
{
    QSqlQuery query;
    query.prepare("INSERT INTO information "
                  "(id,name,age,sex,class,email,finger_id) "
                  "VALUES(:id,:name,:age,:sex,:class,:email,:finger_id)");

    query.bindValue(":id",id);
    query.bindValue(":name",name);
    query.bindValue(":age",age);
    query.bindValue(":sex",sex);
    query.bindValue(":class",schoolClass);
    query.bindValue(":email",email);
    query.bindValue(":finger_id",finger_id);

    bool success = query.exec();

    if(!success)
    {
        qWarning() << "录入学生信息失败：" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::addInformation(const quint16 &finger_id)
{
    QSqlQuery query;

    query.prepare(
        "INSERT INTO information (finger_id) "
        "VALUES (:finger_id)"
        );

    query.bindValue(":finger_id", finger_id);

    if (!query.exec())
    {
        qWarning() << "创建学生信息失败：" << query.lastError().text();
        return false;
    }

    return true;
}

QVariantMap DatabaseManager::getInfoByFinger(quint16 finger_id)
{
    QSqlQuery query;
    QVariantMap info;
    query.prepare("SELECT id,name,age,sex,class,email "
                  "FROM information "
                  "WHERE finger_id = :finger_id");
    query.bindValue(":finger_id",finger_id);

    bool success = query.exec();
    if(!success)
    {
        qWarning() << "查询失败" << query.lastError().text();
        return info;
    }
    success &= query.next();
    if(!success)
    {
        qWarning() << "查询无此人" << query.lastError().text();
        return info;
    }

    info["id"] = query.value("id").toUInt();
    info["name"] = query.value("name").toString();
    info["age"] = query.value("age").toUInt();
    info["sex"] = query.value("sex").toString();
    info["class"] = query.value("class").toString();
    info["email"] = query.value("email").toString();

    return info;
}

bool DatabaseManager::isInfoExistsByFinger(quint16 finger_id)
{
    QSqlQuery query;
    query.prepare("SELECT EXISTS("
                                 "SELECT 1 "
                                 "FROM information "
                                 "WHERE finger_id = :finger_id);");
    query.bindValue(":finger_id",finger_id);
    bool success = query.exec();
    if(!success)
    {
        qWarning() << "查询失败" << query.lastError().text();
        return false;
    }

    success = query.next();

    if(!success)
    {
        qWarning() << "查询结果为空" << query.lastError().text();
    }

    bool exists = query.value(0).toBool();

    return exists;

}

void DatabaseManager::verifyFingerInfo(const QByteArray &data)
{
    quint16 cnt = (data[0]<<8) + data[1];
    for(quint16 i = 1;i<=cnt;i++)
    {
        if(!isInfoExistsByFinger(i))
        {
            addInformation(i);
        }
    }
    emit updateTableView();
}
