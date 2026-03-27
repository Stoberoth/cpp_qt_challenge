#include "../include/MainWindow.hpp"
#include <QVBoxLayout>
#include <QFile>

MainWindow::MainWindow()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    this->setWindowTitle(QString("Todo List"));

    m_lineEdit = new QLineEdit(this);
    m_pushButton = new QPushButton(QString("Ajouter"), this);
    m_saveButton = new QPushButton(QString("Sauvegarder"), this);
    m_listView = new QListView(this);
    m_model = new QStringListModel(this);
    m_listView->setModel(m_model);
    m_fileName = "todos.txt";

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_pushButton);
    layout->addWidget(m_listView);
    layout->addWidget(m_saveButton);
    centralWidget->setLayout(layout);

    loadTasks();

    connect(m_pushButton, &QPushButton::clicked, this, &MainWindow::getLineEditText);
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::saveTasks);
}

MainWindow::~MainWindow()
{
    saveTasks();
}

void MainWindow::getLineEditText()
{
    if(m_lineEdit->text() == "")
    {
        return;
    }
    // on récupére l'existant
    QStringList list = m_model->stringList();
    // on ajoute la nouvelle entrés
    list.append(m_lineEdit->text());
    // on met a jour
    m_model->setStringList(list);
    m_lineEdit->clear();
}

void MainWindow::saveTasks()
{
    QFile saveFile(m_fileName);
    if(saveFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream stream(&saveFile);
        QStringList list = m_model->stringList();
        for(const QString& line : list)
        {
            stream << line << "\n";
        }
        saveFile.close();
    }
}

void MainWindow::loadTasks()
{
    QFile saveFile(m_fileName);
    if(!saveFile.exists())
    {
        return;
    }
    if(saveFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&saveFile);
        QStringList tmpList;
        while (!stream.atEnd())
        {
            tmpList.append(stream.readLine());
        }
        m_model->setStringList(tmpList);
        
        saveFile.close();
    }
}
