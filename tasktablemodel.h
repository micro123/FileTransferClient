#ifndef TASKTABLEMODEL_H
#define TASKTABLEMODEL_H

#include <QAbstractTableModel>

class TaskTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TaskTableModel(QObject *parent = Q_NULLPTR);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData (int section, Qt::Orientation orientation, int role) const override;
    void addTask(QList<QString> files);

    void updateProgress(int percent);
private:
    QList< QPair<QString, int> > taskList;
    int currentTaskIndex;
};

#endif // TASKTABLEMODEL_H
