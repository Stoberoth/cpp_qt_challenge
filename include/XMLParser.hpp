#include <qcontainerfwd.h>

#include <QDomDocument>

#include "TaskData.hpp"

class XMLParser
{
   public:
    static bool saveTasks(QVector<TaskData>& td);
    static QVector<TaskData> loadTasks();
    static QVector<TaskData> streamLoad();
};