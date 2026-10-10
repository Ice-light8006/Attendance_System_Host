#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QDebug>

#define FRAME_HEADER_1 0x5A
#define FRAME_HEADER_2 0XBB
#define FRAME_TAIL_1 0x3B
#define FRAME_TAIL_2 0x4F

class Protocol:public QObject
{
    Q_OBJECT
public:
    Protocol(QObject* parent = nullptr);
    QByteArray Builder(quint8 cmd,const QByteArray &data);
    QList<QByteArray> Parser(const QByteArray &byte);
    void reset();
private slots:
private:
    QByteArray m_buffer;
};

#endif // PROTOCOL_H
