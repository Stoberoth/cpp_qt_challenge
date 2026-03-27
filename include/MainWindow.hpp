#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QListView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QLineEdit *m_lineEdit;
    QPushButton *m_pushButton;
    QPushButton *m_saveButton;
    QListView *m_listView;
    QStringListModel* m_model;
    QString m_fileName;

    void getLineEditText();
    void saveTasks();
    void loadTasks();

public:
    MainWindow();
    ~MainWindow();

protected:
};