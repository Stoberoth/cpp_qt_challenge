#include "../include/MainWindow.hpp"
#include "../include/TaskDelegate.hpp"
#include <QVBoxLayout>
#include <QFile>
#include <QDebug>


MainWindow::MainWindow()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    this->setWindowTitle(QString("Todo List"));

    m_lineEdit = new QLineEdit(this);
    m_pushButton = new QPushButton(QString("Ajouter"), this);
    m_saveButton = new QPushButton(QString("Sauvegarder"), this);
    m_deleteButton = new QPushButton(QString("Supprimer"), this);
    m_deleteButton->setEnabled(false);
    m_listView = new QListView(this);
    m_listView->setItemDelegate(new TaskDelegate(this));
    // set the double click action to edit the item
    // m_listView->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_taskModel = new TaskListModel(this);
    
    m_listView->setModel(m_taskModel);
    m_fileName = "todos.txt";

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_pushButton);
    layout->addWidget(m_listView);
    layout->addWidget(m_saveButton);
    layout->addWidget(m_deleteButton);
    centralWidget->setLayout(layout);

    loadTasks();

    connect(m_pushButton, &QPushButton::clicked, this, &MainWindow::getLineEditText);
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::saveTasks);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedTask);
    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex current)
            { m_deleteButton->setEnabled(current.isValid()); });
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
    TaskData td;
    td.name = m_lineEdit->text();
    td.priority = 1;
    td.createdDate = QDateTime::currentDateTime();
    m_taskModel->addTask(td);
    m_lineEdit->clear();
}

void MainWindow::saveTasks()
{
    QFile saveFile(m_fileName);
    if(saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&saveFile);
        for(int i = 0; i < m_taskModel->rowCount(); ++i)
        {
            stream << m_taskModel->data(m_taskModel->index(i), TaskRoles::NameRole).toString() << " " << m_taskModel->data(m_taskModel->index(i), TaskRoles::PriorityRole).toString() << " " << m_taskModel->data(m_taskModel->index(i), TaskRoles::DateRole).toDateTime().toString(Qt::ISODate) << "\n";
            qDebug() << "save " << m_taskModel->data(m_taskModel->index(i), TaskRoles::NameRole).toString() << " " << m_taskModel->data(m_taskModel->index(i), TaskRoles::PriorityRole).toString() << " " << m_taskModel->data(m_taskModel->index(i), TaskRoles::DateRole).toDateTime().toString(Qt::ISODate).trimmed();
        }
        saveFile.close();
    }
    
}

void MainWindow::loadTasks()
{
    QFile loadFile(m_fileName);
    if (!loadFile.exists())
    {
        return;
    }
    if (loadFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&loadFile);
        QStringList tmpList;
        while (!stream.atEnd())
        {
            auto datas = stream.readLine().split(" ");
            TaskData td;
            td.name = datas[0];
            td.priority = datas[1].toInt();
            td.createdDate = QDateTime::fromString(datas[2].trimmed(), Qt::ISODate);
            qDebug() << "load " << td.name << " " << td.priority << " " << td.createdDate.toString(Qt::ISODate);
            m_taskModel->addTask(td);
        }
        loadFile.close();
    }
}

void MainWindow::deleteSelectedTask()
{
    QModelIndex selectedIndex = m_listView->currentIndex();
    if (selectedIndex.isValid())
    {
        m_taskModel->removeRows(selectedIndex.row(), 1);
        m_deleteButton->setEnabled(m_listView->currentIndex().isValid());
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        deleteSelectedTask();
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}
