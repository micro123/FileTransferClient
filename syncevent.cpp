#include "syncevent.h"

#include <QDataStream>

const QEvent::Type SyncEvent::Sync_Event =
        static_cast<QEvent::Type>(QEvent::registerEventType (0x777));

SyncEvent::SyncEvent(QObject *maker, int type, int reply, QString filename,
                     qint64 length, qint64 progress,
                     QString addition_msg):
    QEvent(Sync_Event),
    MAKER(maker),
    TYPE(type),
    REPLY(reply),
    FileName(filename),
    Length(length),
    PROGRESS(progress),
    ADDITION_MSG(addition_msg)
{

}

SyncEvent::~SyncEvent()
{

}

QDataStream &operator <<(QDataStream &ds, const Header &header)
{
    ds << header.TYPE << header.REPLY << header.FileName << header.Length;
    return ds;
}

QDataStream &operator >>(QDataStream &ds, Header &header)
{
    ds >> header.TYPE >> header.REPLY >> header.FileName >> header.Length;
    return ds;
}

QByteArray headerData(const Header &header)
{
    QByteArray arr;
    QDataStream ds(&arr, QIODevice::WriteOnly);
    ds.setByteOrder (QDataStream::BigEndian);
    ds.setVersion (QDataStream::Qt_5_0);
    ds << qint64(0);
    ds << header;
    ds.device ()->seek (0);
    ds << qint64(arr.size () - sizeof(qint64));
    return arr;
}
