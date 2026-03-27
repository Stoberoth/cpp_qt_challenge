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
    QLineEdit* m_lineEdit;
    QPushButton* m_pushButton;
    QListWidget* m_listWidget;

    inline void getLineEditText(){
        m_listWidget->addItem(m_lineEdit->text());
        m_lineEdit->clear();
    }


    public:
    MainWindow();
    ~MainWindow();


    protected:

};