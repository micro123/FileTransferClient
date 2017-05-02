#include "widget.h"
#include "ui_widget.h"
#include <QtWidgets>
#include <QFileDialog>
#include <QHostAddress>
#include <QDebug>
#include "syncevent.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    isConnected(false)
{
    ui->setupUi(this);
    manager = new TaskManager(this);
    taskModel = new TaskTableModel(this);
    ui->taskTableView->setModel (taskModel);
    ui->taskTableView->horizontalHeader ()->setSectionResizeMode (0, QHeaderView::Stretch);
    connect (manager, &TaskManager::stateChanged, [this](QAbstractSocket::SocketState socketState){
        if (socketState == QAbstractSocket::ConnectedState)
            isConnected = true;
        else
            isConnected = false;
        updateUI ();
    });
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::event(QEvent *event)
{
    if (event->type () != SyncEvent::Sync_Event)
        return QWidget::event (event);
    processEvent (event);
    return true;
}

void Widget::on_connectBtn_clicked()
{
    if (isConnected) {
        if (!taskList.isEmpty ()) {
            QMessageBox::StandardButton btn = QMessageBox::question (this, "确认",
                                                                     "还有未完成的任务，确认关闭连接？");
            if (btn != QMessageBox::Yes)
                return;
        }
        manager->disconnectFromHost ();
        return;
    }
    QHostAddress addr;
    if (!addr.setAddress (ui->serverIPText->text ()))
        return;
    manager->connectToHost (addr, ui->serverPortNum->value ());
}

void Widget::on_addFilesBtn_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames (this, "选择文件", QDir::homePath ());
    if (files.isEmpty ())
        return;
    bool isEmpty = taskList.isEmpty ();
    taskList.append (files);
    taskModel->addTask (files);
    if (isEmpty)
        startTask();
}

void Widget::on_clearBtn_clicked()
{
    // TODO
}

void Widget::on_removeBtn_clicked()
{
    // TODO
}

void Widget::updateUI()
{
    ui->serverIPText->setEnabled (!isConnected);
    ui->serverPortNum->setEnabled (!isConnected);
    ui->addFilesBtn->setEnabled (isConnected);
    ui->statusLable->setText (isConnected ? "已连接上" : "未连接");
    ui->connectBtn->setText (isConnected ? "断开" : "连接");
}

void Widget::processEvent(QEvent *e)
{
    SyncEvent* sync = dynamic_cast<SyncEvent*>(e);
    if (sync->TYPE == TYPE_FINISH) {
        if (!taskList.isEmpty ())
            taskList.removeFirst ();
        startTask ();
    } else if (sync->TYPE == TYPE_UPDATE_PROGRESS) {
        taskModel->updateProgress (sync->PROGRESS * 100 / sync->Length);
    } else if (sync->TYPE == TYPE_REPLY) {
        if (sync->REPLY == REPLY_REFUSE) {
            if (!taskList.isEmpty ())
                taskList.removeFirst ();
            taskModel->updateProgress (101);
            startTask ();
        }

    }
}

void Widget::startTask()
{
    if (!taskList.isEmpty ())
        qApp->postEvent (manager, new SyncEvent(this, TYPE_REQUEST,
                                                REPLY_INVAILD, taskList.front ()),
                         Qt::HighEventPriority);
}
