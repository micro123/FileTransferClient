#include "tasktablemodel.h"

TaskTableModel::TaskTableModel(QObject *parent):
    QAbstractTableModel(parent)
{
    taskList.clear ();
    currentTaskIndex = 0;
}


int TaskTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return taskList.size ();
}

int TaskTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant TaskTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        QVariant var;
        int row = index.row ();
        int val = taskList.at (row).second;
        switch (index.column ()) {
        case 0:
            var = taskList.at (row).first;
            break;
        case 1:
            if (val == -1)
                var = "等待中";
            else if (val == 100)
                var = "已完成";
            else if (val > 100)
                var = "拒绝传输";
            else
                var = QString::asprintf ("上传中：%d %%", val);
            break;
        default:
            var = QVariant();
            break;
        }
        return var;
    }

    return QVariant();
}

QVariant TaskTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return QString("文件名");
        else
            return QString("进度");
    } else {
        return QString::number (section + 1);
    }
}

void TaskTableModel::addTask(QList<QString> files)
{
    beginResetModel ();
    for (int i=0; i<files.size(); i++)
        taskList.append (qMakePair(files[i], -1));
    endResetModel ();
}

void TaskTableModel::updateProgress(int percent)
{
    beginResetModel ();
    taskList[currentTaskIndex].second = percent;
    if (percent >= 100)
        currentTaskIndex++;
    endResetModel ();
}
