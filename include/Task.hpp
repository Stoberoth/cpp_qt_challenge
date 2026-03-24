#include <QObject>
#include <QWeakPointer>
#include <QSharedPointer>
#include <QDebug>
#include <iostream>

class Task : public QObject
{

    Q_OBJECT

private:
    QString m_name;
    bool m_status;
    QWeakPointer<Task> m_next;

public:
    inline Task(QString name, bool completed = false)
    {
        this->m_name = name;
        this->m_status = completed;
    }
    inline ~Task()
    {
        qDebug() << "Destruction de :" << m_name;
    }

    inline QString name()
    {
        return this->m_name;
    }

    inline bool isCompleted()
    {
        return this->m_status;
    }
    inline void setCompleted(bool completed)
    {
        this->m_status = completed;
        emit statusChanged(this->m_name, this->m_status); 
    }

    inline void setNext(QSharedPointer<Task> next)
    {
        m_next = next;
    }
    inline QWeakPointer<Task> next()
    {
        return m_next;
    }

signals:
    void statusChanged(const QString &taskName, bool isCompleted);
};
