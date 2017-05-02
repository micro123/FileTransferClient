#ifndef SYNCEVENT_H
#define SYNCEVENT_H

#include <QEvent>
#include <QString>

// event types
static const int TYPE_REQUEST = 0;
static const int TYPE_REPLY = 1;
static const int TYPE_UPDATE_PROGRESS = 2;
static const int TYPE_FINISH = 3;
static const int TYPE_INVAILD = 4;

// reply types
static const int REPLY_REFUSE = 0;
static const int REPLY_COMFIRM = 1;
static const int REPLY_INVAILD = 2;

class SyncEvent : public QEvent
{
public:
    SyncEvent(QObject *maker, int type = TYPE_INVAILD, int reply = REPLY_INVAILD,
            QString filename = QString(), qint64 length = 0,
              qint64 progress = 0, QString addition_msg = QString());
    ~SyncEvent();

    QObject *MAKER;

    // the event type
    int TYPE;

    // reply, available for the TYPE is TYPE_REPLY
    int REPLY;

    // TaskInfo, available for the TYPE is TYPE_REQUEST
    QString FileName;
    qint64 Length;

    // bytes readed, available for the TYPE is TYPE_UPDATE_PROGRESS
    qint64 PROGRESS;

    // additional message, all types can use
    QString ADDITION_MSG;

    static const Type Sync_Event;
};

//static const QEvent::Type Sync_Event = static_cast<QEvent::Type>(QEvent::registerEventType (0x787));

struct Header {
    int TYPE;
    int REPLY;
    qint64 Length;
    QString FileName;
};

QDataStream& operator << (QDataStream& ds, const Header& header);
QDataStream& operator >> (QDataStream& ds, Header& header);

QByteArray headerData(const Header &header);

#endif // SYNCEVENT_H
