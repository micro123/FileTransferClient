#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QTcpSocket>

class TaskManager : public QTcpSocket
{
    Q_OBJECT
public:
    TaskManager(QObject *eventWatcher);
    ~TaskManager();

    bool event (QEvent *event) override;
private:
    QObject *const eventWatcher;
    QThread *mThread, *structureThread;
    QDataStream *mDataStream;

    bool hasHeaderData;
    bool isTransfering;
    qint64 headerLength;
    QString currentFile;
    qint64 currentFileLength;
    qint64 currentFileSent;

    void processEvent(QEvent* e);
    void sendEventToWatcher(QEvent* e);
    void writeRequestHeader(QString fileName);
    void readHeader();
    void transferFile();
    void finishTask(bool refused = false);
private slots:
    void onReadyRead();
};

#endif // TASKMANAGER_H
