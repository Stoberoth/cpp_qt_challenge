#include "../include/TaskData.hpp"

TaskListModel::TaskListModel(QObject* parent) : QAbstractListModel(parent){}

int TaskListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_task.size();
}

QVariant TaskListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_task.size())
        return QVariant();
    const TaskData& task = m_task.at(index.row());
    switch (role) {
        case NameRole:     return task.name;
        case PriorityRole: return task.priority;
        case DateRole:     return task.createdDate;
        default:           return QVariant();
    }
}
Qt::ItemFlags TaskListModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool TaskListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if(row < 0 || row > m_task.size())
    {
        return false;
    }
    // on dis que l'on veut ajouter count lignes depuis row
    beginInsertRows(parent, row, row + count - 1);

    for (int i = 0; i < count; i++)
    {
        m_task.insert(row, TaskData{});
    }
    endInsertRows();
    return true;
}

bool TaskListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (row < 0 || count <= 0 || row + count > m_task.size())
    {
        return false;
    }
    // on dis que l'on veut surpprimer count lignes depuis row
    beginRemoveRows(parent, row, row + count - 1);

    m_task.remove(row, count);

    endRemoveRows();
    return true;
}

void TaskListModel::addTask(const TaskData& task)
{
    insertRows(m_task.size(), 1);       // insère une ligne vide à la fin
    m_task[m_task.size() - 1] = task;  // remplit avec les données
}
