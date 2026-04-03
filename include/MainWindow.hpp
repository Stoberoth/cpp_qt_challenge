#pragma once

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QListView>
#include <QKeyEvent>
#include "../include/TaskData.hpp"
#include "../include/DataLoader.hpp"
#include <QThread>
#include <QProgressBar>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QLabel>
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QLineEdit *m_lineEdit;
    QPushButton *m_pushButton;
    QPushButton *m_deleteButton;
    QPushButton *m_saveButton;
    QPushButton *m_ascendingButton;
    QPushButton *m_descendingButton;
    QPushButton *m_loadButton;
    QListView *m_listView;
    QSortFilterProxyModel* m_model;

    QLabel* m_loadingLabel;
    QTimer* m_timeoutTimer;

    TaskListModel* m_taskModel;

    QNetworkAccessManager* m_networkManager;

    QThread* m_thread;

    DataLoader* m_dataLoader;

    QProgressBar* m_progressBar;

    void setupWidgets();
    void setupWorkers();
    void setupModel();
    void setupConnections();
    void keyPressEvent(QKeyEvent* event) override;
    void fetchTasksFromServer();

    
   
private slots:

    
    void saveTasks();
    void loadTasks();
    void getLineEditText();
    void deleteSelectedTask();
    void filterOrder(bool ascending);
    void parseReply(QNetworkReply* reply);



public:
    MainWindow();
    ~MainWindow();

};