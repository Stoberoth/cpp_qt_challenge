#include "XMLParser.hpp"

#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qdom.h>
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>

#include <QDebug>
#include <QXmlStreamReader>

#include "QFile"
#include "TaskData.hpp"

bool XMLParser::saveTasks(QVector<TaskData>& td)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("tasks");
    doc.appendChild(root);
    for (TaskData t : td)
    {
        QDomElement task = doc.createElement("task");
        task.setAttribute("id", t.id);
        task.setAttribute("name", t.name);
        task.setAttribute("completed", t.completed);
        task.setAttribute("createdDate", t.createdDate.toString(Qt::ISODate));
        task.setAttribute("priority", t.priority);
        root.appendChild(task);
    }
    QFile file("task.xml");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "save in xml";
        QTextStream out(&file);
        out << doc.toString();
        file.close();
        return true;
    }
    file.close();
    return false;
}

QVector<TaskData> XMLParser::loadTasks()
{
    QDomDocument doc;
    QFile file("task.xml");
    QVector<TaskData> tasksVector;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw;
    }
    doc.setContent(file.readAll());
    QDomElement root = doc.documentElement();
    QDomNodeList tasks = root.elementsByTagName("task");
    for (QDomNode node : tasks)
    {
        TaskData td;
        td.name = node.toElement().attribute("name");
        td.completed = node.toElement().attribute("completed").toInt();
        td.id = node.toElement().attribute("id").toInt();
        td.priority = node.toElement().attribute("priority").toInt();
        td.createdDate = QDateTime::fromString(node.toElement().attribute("createdDate"), Qt::ISODate);
        tasksVector.append(td);
    }
    file.close();
    return tasksVector;
}

QVector<TaskData> XMLParser::streamLoad()
{
    QFile file("task.xml");
    QVector<TaskData> tasksVector;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw;
    }
    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError())
    {
        reader.readNext();
        if (reader.isStartElement() && reader.name() == "task")
        {
            TaskData t;
            const auto attrs = reader.attributes();

            t.id = attrs.value("id").toInt();
            t.name = attrs.value("name").toString();
            t.completed = attrs.value("completed").toInt();
            t.priority = attrs.value("priority").toInt();
            t.createdDate = QDateTime::fromString(attrs.value("createdDate").toString(), Qt::ISODate);
            tasksVector.push_back(t);
        }
    }
    if (reader.hasError())
    {
        qWarning() << "Erreur XML: " << reader.errorString() << "(ligne" << reader.lineNumber() << ")";
    }
    file.close();
    return tasksVector;
}