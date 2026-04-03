#include "../include/DataLoader.hpp"

#include <QThread>


DataLoader::DataLoader(QObject* parent) : QObject(parent)
{
}

void DataLoader::loadData()
{
    auto results = QStringList();
    for (int i = 0; i <= 10; ++i)
    {
        QThread::msleep(100);
        emit progressUpdated(i * 10);
    }
    emit dataLoaded(results);
}