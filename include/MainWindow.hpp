#pragma once

#include <qpushbutton.h>

#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QThread>
#include <QTimer>
#include <QWidget>

#include "DataLoader.hpp"
#include "TaskData.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

   private:
    QLineEdit* m_lineEdit;
    QPushButton* m_pushButton;
    QPushButton* m_deleteButton;
    QPushButton* m_saveButton;
    QPushButton* m_ascendingButton;
    QPushButton* m_descendingButton;
    QPushButton* m_loadButton;
    QPushButton* m_settingsButton;
    QListView* m_listView;
    QSortFilterProxyModel* m_model;

    QLabel* m_loadingLabel;
    QTimer* m_timeoutTimer;

    TaskListModel* m_taskModel;

    QNetworkAccessManager* m_networkManager;

    QThread* m_thread;

    DataLoader* m_dataLoader;

    QProgressBar* m_progressBar;

    QSettings* m_settings;
    void setupWidgets();
    void setupWorkers();
    void setupModel();
    void setupConnections();
    void keyPressEvent(QKeyEvent* event) override;
    void fetchTasksFromServer();

    bool createDatabase();

    void createTaskInDatabase(TaskData& td);
    void getTasksInDatabase();
    void updateTasksInDatabase();
    void deleteTasksInDatabase();

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