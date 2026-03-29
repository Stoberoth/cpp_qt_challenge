#pragma once

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QListView>
#include <QKeyEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QLineEdit *m_lineEdit;
    QPushButton *m_pushButton;
    QPushButton *m_deleteButton;
    QPushButton *m_saveButton;
    QListView *m_listView;
    QStringListModel* m_model;
    QString m_fileName;

    

    void keyPressEvent(QKeyEvent* event) override;
   
private slots:
    void saveTasks();
    void loadTasks();
    void getLineEditText();
    void deleteSelectedTask();


public:
    MainWindow();
    ~MainWindow();

};