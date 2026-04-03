#pragma once

#include <QAbstractListModel>
#include <QDateTime>

enum TaskRoles
{
    NameRole = Qt::UserRole + 1,
    PriorityRole,
    DateRole,
    CompletedRole
};

struct TaskData
{
    QString name;
    int priority = 0;
    QDateTime createdDate;
    bool completed = false;
};

class TaskListModel : public QAbstractListModel
{
    Q_OBJECT

   signals:
    void taskAdded(int index, const QString& name);
    void taskRemoved(int index);
    void taskPriorityChanged(int index, int newPriority);

   public:
    TaskListModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    void saveTasksToJson();
    void loadTasksFromJson();
   public slots:
    void addTask(const TaskData& task);

   private:
    QVector<TaskData> m_task;
};