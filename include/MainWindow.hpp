#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QLineEdit *m_lineEdit;
    QPushButton *m_pushButton;
    QPushButton *m_saveButton;
    QListWidget *m_listWidget;
    QString m_fileName;

    void getLineEditText();
    void saveTasks();
    void loadTasks();

public:
    MainWindow();
    ~MainWindow();

protected:
};