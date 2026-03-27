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
    m_listWidget = new QListWidget(this);

    m_fileName = "todos.txt";

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_pushButton);
    layout->addWidget(m_listWidget);
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
    m_listWidget->addItem(m_lineEdit->text());
    m_lineEdit->clear();
}

void MainWindow::saveTasks()
{
    QFile saveFile(m_fileName);
    if(saveFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream stream(&saveFile);
        for(int i = 0; i < m_listWidget->count(); ++i)
        {
            stream << m_listWidget->item(i)->text() << "\n";
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
        while (!stream.atEnd())
        {
            m_listWidget->addItem(stream.readLine());
        }
        
        saveFile.close();
    }
}
