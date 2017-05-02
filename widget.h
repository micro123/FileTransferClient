#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "taskmanager.h"
#include "tasktablemodel.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    bool event (QEvent *event);
private slots:
    void on_connectBtn_clicked();

    void on_addFilesBtn_clicked();

    void on_clearBtn_clicked();

    void on_removeBtn_clicked();

private:
    Ui::Widget *ui;
    bool isConnected;
    TaskManager *manager;
    QList<QString> taskList;
    TaskTableModel *taskModel;

    void updateUI ();
    void processEvent(QEvent* e);
    void startTask();
};

#endif // WIDGET_H
