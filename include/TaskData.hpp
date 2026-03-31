#pragma once

#include <QAbstractListModel>
#include <QDateTime>

enum TaskRoles {
    NameRole = Qt::UserRole + 1,
    PriorityRole,
    DateRole
};

struct TaskData {
    QString name;
    int priority;
    QDateTime createdDate;
};

class TaskListModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        TaskListModel(QObject* parent = nullptr);
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
        bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    public slots:
        void addTask(const TaskData& task);
    private:

        QVector<TaskData> m_task;

    signals:
        void taskAdded(int index, const QString& name);
        void taskRemoved(int index);
        void taskPriorityChanged(int index, int newPriority);

};