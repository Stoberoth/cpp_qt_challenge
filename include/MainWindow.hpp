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
    TaskListModel* m_taskModel;

    QThread* m_thread;

    DataLoader* m_dataLoader;

    QProgressBar* m_progressBar;

    QString m_fileName;

    

    void keyPressEvent(QKeyEvent* event) override;
   
private slots:
    void saveTasks();
    void loadTasks();
    void getLineEditText();
    void deleteSelectedTask();
    void filterOrder(bool ascending);


public:
    MainWindow();
    ~MainWindow();

};