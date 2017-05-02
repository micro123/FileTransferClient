#include "taskmanager.h"
#include <QDataStream>
#include <QThread>
#include <QApplication>
#include <QQueue>
#include <QFileInfo>

#include "syncevent.h"

TaskManager::TaskManager(QObject *eventWatcher):
    QTcpSocket(0),
    eventWatcher(eventWatcher)
{
    structureThread = QThread::currentThread ();
    mThread = new QThread;
    mDataStream = new QDataStream(this);
    isTransfering = false;
    hasHeaderData = false;
    headerLength = 0;

    connect (this, &TaskManager::readyRead, this, &TaskManager::onReadyRead);

    moveToThread (mThread);
    mThread->start ();
}

TaskManager::~TaskManager()
{
    moveToThread (structureThread);
    mThread->quit ();
    delete mThread;
}

bool TaskManager::event(QEvent *event)
{
    qDebug() << event->type ();
    if (event->type () != SyncEvent::Sync_Event)
        return QTcpSocket::event (event);
    else
        processEvent (event);
    return true;
}

void TaskManager::processEvent(QEvent *e)
{
    if (e->type () == SyncEvent::Sync_Event) {
        SyncEvent* event = dynamic_cast<SyncEvent*>(e);
        if (event->TYPE == TYPE_REQUEST)
            writeRequestHeader(event->FileName);
    }
}

void TaskManager::sendEventToWatcher(QEvent *e)
{
    if (!eventWatcher)
        return;
    qApp->postEvent (eventWatcher, e, Qt::HighEventPriority);
}

void TaskManager::writeRequestHeader(QString fileName)
{
    qDebug() << "writeRequestHeader" << fileName;
    Header header;
    QFileInfo info(fileName);
    header.TYPE = TYPE_REQUEST;
    header.REPLY = REPLY_INVAILD;
    header.FileName = info.fileName ();
    header.Length = currentFileLength = info.size ();
    currentFileSent = 0;
    currentFile = fileName;
    write (headerData (header));
    flush ();
}

void TaskManager::readHeader()
{
    qDebug() << "readHead";
    if (!headerLength) {
        if (bytesAvailable () >= (qint64)sizeof(qint64))
            (*mDataStream) >> headerLength;
    }
    if (bytesAvailable () >= headerLength) {
        Header header;
        (*mDataStream) >> header;
        qDebug() << header.TYPE << header.REPLY;
//        if (header.TYPE == TYPE_REPLY && header.REPLY == REPLY_COMFIRM) {
//            isTransfering = true;
//            hasHeaderData = true;
//            transferFile ();
//        } else {
//            isTransfering = false;
//            hasHeaderData = false;
//            finishTask ();
//        }
        if (header.REPLY == REPLY_COMFIRM) {
            isTransfering = true;
            transferFile ();
        } else if (header.REPLY == REPLY_REFUSE) {
            isTransfering = false;
            hasHeaderData = false;
            finishTask (true);
        } else if (header.TYPE == TYPE_FINISH) {
            isTransfering = false;
            hasHeaderData = false;
            finishTask ();
        }
    }
}

void TaskManager::transferFile()
{
    qDebug() << "transferFile";
    hasHeaderData = false;
    headerLength = 0;
    isTransfering = false;

    QFile file(currentFile);
    if (file.open (QIODevice::ReadOnly)) {
        while (currentFileSent < currentFileLength) {
            QByteArray arr = file.read ((currentFileLength - currentFileSent > 64*1024) ?
                                             64*1024 : currentFileLength - currentFileSent);
            currentFileSent += arr.size ();
//            mDataStream->device ()->write (arr);
            write (arr);
            waitForBytesWritten ();
            flush ();
            sendEventToWatcher (new SyncEvent(this, TYPE_UPDATE_PROGRESS,
                                              REPLY_INVAILD, currentFile,
                                              currentFileLength, currentFileSent));
        }
        file.close ();
        qDebug() << "writed";
    }
}

void TaskManager::finishTask(bool refused)
{
    hasHeaderData = false;
    headerLength = 0;
    isTransfering = false;
    if (refused)
        sendEventToWatcher (new SyncEvent(this, TYPE_REPLY, REPLY_REFUSE,
                                      currentFile, currentFileLength, currentFileSent));
    else
        sendEventToWatcher (new SyncEvent(this, TYPE_FINISH, REPLY_INVAILD,
                                      currentFile, currentFileLength, currentFileSent));
}

void TaskManager::onReadyRead()
{
    readHeader ();
}
