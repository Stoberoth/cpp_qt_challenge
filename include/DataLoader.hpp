#pragma once

#include <QObject>

class DataLoader : public QObject
{
    Q_OBJECT

   public:
    DataLoader(QObject* parent = nullptr);
   public slots:
    void loadData();

   signals:
    void progressUpdated(int percent);
    void dataLoaded(const QStringList& data);
};