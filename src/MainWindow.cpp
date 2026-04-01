#include "../include/MainWindow.hpp"
#include "../include/TaskDelegate.hpp"
#include <QVBoxLayout>
#include <QFile>
#include <QStatusBar>
#include <iostream>
#include <QUrl>
#include <QPointer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>



MainWindow::MainWindow()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    this->setWindowTitle(QString("Todo List"));
    
    m_lineEdit = new QLineEdit(this);
    m_pushButton = new QPushButton(QString("Ajouter"), this);
    m_saveButton = new QPushButton(QString("Sauvegarder"), this);
    m_deleteButton = new QPushButton(QString("Supprimer"), this);
    m_ascendingButton = new QPushButton(QString("Ordre croissant"), this);
    m_descendingButton = new QPushButton(QString("Ordre descroissant"), this);
    m_loadButton = new QPushButton(QString("Load Data"), this);

    m_thread = new QThread(this);
    m_dataLoader = new DataLoader();

    m_loadingLabel = new QLabel("Chargement ...", this);
    m_loadingLabel->setAlignment(Qt::AlignCenter);
    m_loadingLabel->setStyleSheet("color: orange; font-weight: bold;");
    m_loadingLabel->hide();

    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(5000);

    // manage the http request
    m_networkManager = new QNetworkAccessManager(this);

    m_progressBar = new QProgressBar(this);

    m_dataLoader->moveToThread(m_thread);

    m_deleteButton->setEnabled(false);
    m_listView = new QListView(this);
    m_listView->setItemDelegate(new TaskDelegate(this));
    // set the double click action to edit the item
    // m_listView->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_taskModel = new TaskListModel(this);
    m_model = new QSortFilterProxyModel(this);
    m_listView->setModel(m_model);

    m_model->setSourceModel(m_taskModel);
    // Indiquer quel rôle utiliser pour le tri/filtre
    m_model->setFilterRole(TaskRoles::NameRole);
    m_model->setSortRole(TaskRoles::NameRole);
    m_model->sort(0, Qt::DescendingOrder);

    m_fileName = "todos.txt";

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_pushButton);
    layout->addWidget(m_listView);
    layout->addWidget(m_saveButton);
    layout->addWidget(m_loadButton);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_ascendingButton);
    layout->addWidget(m_descendingButton);
    layout->addWidget(m_progressBar);
    layout->addWidget(m_loadingLabel);
    centralWidget->setLayout(layout);

    //loadTasks();

    connect(m_pushButton, &QPushButton::clicked, this, &MainWindow::getLineEditText);
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::saveTasks);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedTask);
    connect(m_ascendingButton, &QPushButton::clicked, this, [this](){
        filterOrder(true);
    });
    connect(m_descendingButton, &QPushButton::clicked, this, [this](){
        filterOrder(false);
    });
    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex current)
            { m_deleteButton->setEnabled(current.isValid()); });
    connect(m_taskModel, &TaskListModel::taskAdded, this, [this](int index, const QString& name) {
        statusBar()->showMessage(QString("Added: %1").arg(name));
    });
    connect(m_taskModel, &TaskListModel::taskRemoved, this, [this](int index) {
        statusBar()->showMessage(QString("Removed: row number %1").arg(index));
    });
    connect(m_thread, QThread::started, m_dataLoader, &DataLoader::loadData);

    connect(m_dataLoader, &DataLoader::progressUpdated, m_progressBar, &QProgressBar::setValue);
    connect(m_dataLoader, &DataLoader::dataLoaded, this, &MainWindow::loadTasks);
    connect(m_dataLoader, &DataLoader::dataLoaded, m_thread, &QThread::quit);

    connect(m_loadButton, &QPushButton::clicked, m_thread, [this](){
        if(m_thread->isRunning())
        {
            return;
        }
        m_thread->start();
    });


    //fetchTasksFromServer();
}

MainWindow::~MainWindow()
{
    disconnect(m_pushButton, &QPushButton::clicked, this, nullptr);
    disconnect(m_saveButton, &QPushButton::clicked, this, nullptr);
    disconnect(m_deleteButton, &QPushButton::clicked, this, nullptr);
    disconnect(m_ascendingButton, &QPushButton::clicked, this, nullptr);
    disconnect(m_descendingButton, &QPushButton::clicked, this, nullptr);
    disconnect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged, this, nullptr);
    disconnect(m_taskModel, &TaskListModel::taskAdded, this, nullptr);
    disconnect(m_taskModel, &TaskListModel::taskRemoved, this, nullptr);
    m_thread->quit();
    m_thread->wait();
    delete m_dataLoader;
    
    saveTasks();
}

void MainWindow::fetchTasksFromServer()
{
    QNetworkRequest request(QUrl(QString("https://jsonplaceholder.typicode.com/todos")));
    m_loadingLabel->show();
    m_timeoutTimer->start();
    QNetworkReply* reply = m_networkManager->get(request);

    // on créer un observer pour vérifier que le pointer existe encore dans le cas ou les requêtes partirais de manière trop proche
    QPointer<QNetworkReply> safeReply = reply;

    // évite multiple connection avec des reply qui n'existe plus
    m_timeoutTimer->disconnect();
    
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        m_timeoutTimer->stop();
        parseReply(reply);
        reply->deleteLater();
    });

    connect(m_timeoutTimer, &QTimer::timeout, this, [this, safeReply]() {
        if(safeReply){
            safeReply->abort();
            m_loadingLabel->setText("Timeout — serveur trop long");
        }
    });

    
}

void MainWindow::parseReply(QNetworkReply* reply)
{
    
    if(reply->error() != 0)
    {
        std::cout << "erreur" << std::endl;
        return;
    }
    m_loadingLabel->hide();
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray dataArray = doc.array();
    for(QJsonValue value : dataArray)
    {
        QJsonObject obj = value.toObject();
        QString title = obj["title"].toString();
        TaskData td;
        td.name = title;
        td.priority = 1;
        m_taskModel->addTask(td);
    }
    
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
    /*
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
    */
    m_taskModel->saveTasksToJson();
}

void MainWindow::loadTasks()
{
    /*QFile loadFile(m_fileName);
    if (!loadFile.exists())
    {
        return;
    }
    if (loadFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_taskModel->removeRows(0, m_taskModel->rowCount());
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
    }*/
    m_taskModel->loadTaskFromJson();
}

void MainWindow::deleteSelectedTask()
{
    QModelIndex selectedIndex = m_listView->currentIndex();
    if (selectedIndex.isValid())
    {   
        // on map l'index proxy avec le vrai index du model de référence
        QModelIndex source = m_model->mapToSource(selectedIndex);
        m_taskModel->removeRows(source.row(), 1);
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

void MainWindow::filterOrder(bool ascending)
{
    if(ascending)
    {
        m_model->sort(0,Qt::AscendingOrder);
    }
    else
    {
        m_model->sort(0,Qt::DescendingOrder);
    }
}
