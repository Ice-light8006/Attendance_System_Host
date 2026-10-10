#include "Protocol.h"

Protocol::Protocol(QObject *parent)
    :QObject(parent)
{

}

void Protocol::reset()
{
    m_buffer.clear();
}

QByteArray Protocol::Builder(quint8 cmd, const QByteArray &data)
{
    QByteArray frame;
    frame.append((char)FRAME_HEADER_1);
    frame.append((char)FRAME_HEADER_2);
    frame.append((char)cmd);
    frame.append((char)data.size());

    if(!data.isEmpty()){
        frame.append(data);
    }

    frame.append((char)FRAME_TAIL_1);
    frame.append((char)FRAME_TAIL_2);

    return frame;
}

// ========== 拆包函数 ==========
QList<QByteArray> Protocol::Parser(const QByteArray &byte)
{
    m_buffer.append(byte);
    QList<QByteArray> result;

    // 拼装用于查找的帧头
    QByteArray headerSeq;
    headerSeq.append((char)FRAME_HEADER_1);
    headerSeq.append((char)FRAME_HEADER_2);

    while(1)
    {
        // 1. 在缓冲区中查找帧头
        int headIdx = m_buffer.indexOf(headerSeq);
        if(headIdx == -1)
        {
            // 防止 FRAME_HEADER_1 (0x5A) 刚好被截断在缓冲区的最后一位
            if(m_buffer.endsWith((char)FRAME_HEADER_1)) {
                m_buffer = m_buffer.right(1);
            } else {
                m_buffer.clear();
            }
            break;
        }

        // 2. 清理帧头前面的垃圾数据
        if(headIdx > 0)
        {
            m_buffer.remove(0, headIdx);
        }

        // 3. 检查总长度够不够最小包 (头2 + cmd1 + len1 + 尾2 = 6)
        if(m_buffer.size() < 6)
        {
            break;
        }

        // 4. 提取数据长度
        quint8 len = (quint8)m_buffer[3];
        int frameTotalSize = 6 + len;

        // 5. 检查是否收全了这整帧
        if(m_buffer.size() < frameTotalSize)
        {
            break;
        }

        // 6. 核对帧尾
        if((quint8)m_buffer[frameTotalSize - 2] == FRAME_TAIL_1 &&
            (quint8)m_buffer[frameTotalSize - 1] == FRAME_TAIL_2)
        {
            // 校验成功，提取指令码和数据
            quint8 cmd = (quint8)m_buffer[2];
            QByteArray data = m_buffer.mid(4, len);

            // 组装成 [CMD][DATA...] 的形式返回给上层
            QByteArray packet;
            packet.append((char)cmd);
            packet.append(data);
            result.append(packet);

            // 移除已处理完的这一帧
            m_buffer.remove(0, frameTotalSize);
        }
        else
        {
            // 帧尾不对，说明遇到假帧头，丢弃头部2字节重新找
            m_buffer.remove(0, 2);
        }
    }
    return result;
}
