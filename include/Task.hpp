#pragma once

#include <QDebug>
#include <QObject>
#include <QSharedPointer>
#include <QWeakPointer>


class Task : public QObject
{
    Q_OBJECT

   private:
    QString m_name;
    bool m_status;
    QWeakPointer<Task> m_next;

   public:
    Task(const QString& name, bool completed = false) : m_name(name), m_status(completed)
    {
    }
    ~Task()
    {
        qDebug() << "Destruction de :" << m_name;
    }

    QString name()
    {
        return m_name;
    }

    bool isCompleted()
    {
        return m_status;
    }
    void setCompleted(bool completed)
    {
        m_status = completed;
        emit statusChanged(this->m_name, this->m_status);
    }

    void setNext(const QSharedPointer<Task>& next)
    {
        m_next = next;
    }
    QWeakPointer<Task> next()
    {
        return m_next;
    }

   signals:
    void statusChanged(const QString& taskName, bool isCompleted);
};
