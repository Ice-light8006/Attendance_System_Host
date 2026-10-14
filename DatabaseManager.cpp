#include "DatabaseManager.h"
#include "StudentProtocol.h"

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{

}

static QVector<bool> makeFingerTable(const QByteArray &data)
{
    QVector<bool> tmp;
    tmp.resize(1024);
    for(int i = 0;i<128;i++)
    {
        for(int j = 0;j<8;j++)
        {
            tmp[i*8+j] = (data[i]>>j) & 1;
        }
    }
    return tmp;
}

static QVector<bool> makeFingerTable(const QByteArray &data,quint16* cnt)
{
    int count = 0;
    QVector<bool> tmp;
    tmp.resize(1024);
    for(int i = 0;i<128;i++)
    {
        for(int j = 0;j<8;j++)
        {
            tmp[i*8+j] = (data[i]>>j) & 1;
            count += (data[i]>>j) & 1;
        }
    }
    *cnt = count;
    return tmp;
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

    info["id"] = query.value("id");
    info["name"] = query.value("name").toString();
    info["age"] = query.value("age");
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

void DatabaseManager::updateFingerTable(const QByteArray &data)
{
    QVector<bool> fingerTable = makeFingerTable(data);
    for(quint16 i = 0;i<1024;i++)
    {
        if(fingerTable[i] && !isInfoExistsByFinger(i))
        {
            addInformation(i);
        }
    }
    emit updateTableView();
}

void DatabaseManager::sendStudentInfoByFinger(quint16 finger_id)
{
    QByteArray data;
    data.append(static_cast<char>(STUDENT_INFO_VERSION));
    data.append(static_cast<char>(STUDENT_INFO_OK));
    data.append(static_cast<char>(finger_id >> 8));
    data.append(static_cast<char>(finger_id & 0xFF));
    auto fail = [&](quint8 status) {
        data.resize(4);
        data[1] = static_cast<char>(status);
        emit studentInfoReady(data);
    };
    if (finger_id >= STUDENT_INFO_MAX_FINGER) {
        fail(STUDENT_INFO_INVALID);
        return;
    }
    if (!db.isOpen()) {
        fail(STUDENT_INFO_DB_ERROR);
        return;
    }
    QSqlQuery query(db);
    query.prepare("SELECT id,name,age,sex,class,email FROM information WHERE finger_id=:finger_id");
    query.bindValue(":finger_id", finger_id);
    if (!query.exec()) {
        qWarning() << "Student query failed:" << query.lastError().text();
        fail(STUDENT_INFO_DB_ERROR);
        return;
    }
    if (!query.next()) {
        fail(query.lastError().isValid() ? STUDENT_INFO_DB_ERROR : STUDENT_INFO_NOT_FOUND);
        return;
    }
    bool idOk = false;
    bool ageOk = false;
    const qlonglong id = query.value("id").toLongLong(&idOk);
    const QVariant ageValue = query.value("age");
    const bool ageMissing = ageValue.isNull() || ageValue.toString().trimmed().isEmpty();
    const int age = ageMissing ? STUDENT_INFO_AGE_UNKNOWN : ageValue.toInt(&ageOk);
    // Existing records may have only some personal fields filled in.
    if (!idOk || id < 0 || id > 0xFFFFFFFFLL ||
        (!ageMissing && (!ageOk || age < 0 || age >= STUDENT_INFO_AGE_UNKNOWN))) {
        fail(STUDENT_INFO_INVALID);
        return;
    }
    const quint32 studentId = static_cast<quint32>(id);
    for (int shift = 24; shift >= 0; shift -= 8)
        data.append(static_cast<char>((studentId >> shift) & 0xFF));
    data.append(static_cast<char>(age));
    const char *fields[] = {"name", "sex", "class", "email"};
    for (const char *field : fields) {
        const QString value = query.value(field).toString();
        const QByteArray text = value.trimmed().isEmpty() ? QByteArray() : value.toUtf8();
        if (text.contains(char(0))) {
            fail(STUDENT_INFO_INVALID);
            return;
        }
        if (text.size() > 255 || data.size() + 1 + text.size() > STUDENT_INFO_MAX_PAYLOAD) {
            fail(STUDENT_INFO_TOO_LONG);
            return;
        }
        data.append(static_cast<char>(text.size()));
        data.append(text);
    }
    emit studentInfoReady(data);
}
