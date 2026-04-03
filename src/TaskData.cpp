#include "../include/TaskData.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

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
        case CompletedRole: return task.completed;
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
    emit taskRemoved(row);

    endRemoveRows();
    return true;
}

void TaskListModel::addTask(const TaskData& task)
{
    insertRows(m_task.size(), 1);       // insère une ligne vide à la fin
    m_task[m_task.size() - 1] = task;  // remplit avec les données
    emit taskAdded(m_task.size() - 1, task.name);
}


void TaskListModel::saveTasksToJson()
{
    QJsonArray taskArray;
    for(TaskData td : m_task)
    {
        QJsonObject taskObj;
        taskObj["name"] = td.name;
        taskObj["priority"] = td.priority;
        taskObj["createdDate"] = td.createdDate.toString(Qt::ISODate);
        taskObj["completed"] = td.completed;
        taskArray.push_back(taskObj);
    }

    QJsonDocument doc(taskArray);
    QFile file("json.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(doc.toJson(QJsonDocument::Indented));
    }
    file.close();
}

void TaskListModel::loadTasksFromJson()
{
    QFile file("json.txt");
    if(!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Fichier Introuvable";
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError)
    {
        qWarning() << "Erreur parsing :" << parseError.errorString(); 
    }

    if(!doc.isArray())
    {
        qWarning() << "JSON attendu : pas de tableau";
        return;
    }

    for (const QJsonValue& val : doc.array())
    {
        if(!val.isObject()) continue;
        QJsonObject obj = val.toObject();

        if (!obj.contains("name") || !obj.contains("priority") || !obj.contains("createdDate") || !obj.contains("completed"))
        {
            qWarning() << "Objet incomplet dans le fichier, objet ignoré";
            continue;
        }

        TaskData td;
        td.name = obj["name"].toString();
        td.priority = obj["priority"].toInt();
        td.createdDate = QDateTime::fromString(obj["createdDate"].toString(),Qt::ISODate);
        td.completed = obj["completed"].toBool();
        addTask(td);
    }


}