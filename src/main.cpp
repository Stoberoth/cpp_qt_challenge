#include <iostream>
#include <QCoreApplication>
#include <QDebug>
#include "../include/Task.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    auto t1 = QSharedPointer<Task>::create("Apprendre le C++");
    auto t2 = QSharedPointer<Task>::create("Maitriser Qt");
    auto t3 = QSharedPointer<Task>::create("Trouver une mission");
    t1->setNext(t2);
    t2->setNext(t3);

    QObject::connect(t1.data(), &Task::statusChanged, [](const QString &taskName)
                     { qDebug() << "Signal émis: " << taskName.toStdString()
                                << " a été complété\n"; });

    t1->setCompleted(true);
    t1.reset();
    t2.reset();
    t3.reset();

    return a.exec();
}
