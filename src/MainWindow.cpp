#include "../include/MainWindow.hpp"

#include <qabstractitemmodel.h>
#include <qdatetime.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlquery.h>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointer>
#include <QSql>
#include <QStatusBar>
#include <QUrl>
#include <QVBoxLayout>

#include "PreferenceDialog.hpp"
#include "TaskDelegate.hpp"
#include "XMLParser.hpp"

MainWindow::MainWindow()
{
    this->setWindowTitle(QString("Todo List"));
    this->setupWidgets();
    this->setupWorkers();
    this->setupModel();
    this->setupConnections();

    qWarning() << createDatabase();

    resize(m_settings->value("window/size", QSize(800, 600)).toSize());
    move(m_settings->value("window/pos", QPoint(200, 200)).toPoint());

    // fetchTasksFromServer();
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

    m_settings->setValue("window/size", size());
    m_settings->setValue("window/pos", pos());

    saveTasks();
}

void MainWindow::setupWidgets()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    m_lineEdit = new QLineEdit(this);
    m_pushButton = new QPushButton(QString("Ajouter"), this);
    m_saveButton = new QPushButton(QString("Sauvegarder"), this);
    m_deleteButton = new QPushButton(QString("Supprimer"), this);
    m_ascendingButton = new QPushButton(QString("Ordre croissant"), this);
    m_descendingButton = new QPushButton(QString("Ordre descroissant"), this);
    m_loadButton = new QPushButton(QString("Load Data"), this);
    m_settingsButton = new QPushButton(QString("Préférence"), this);

    m_loadingLabel = new QLabel("Chargement ...", this);
    m_loadingLabel->setAlignment(Qt::AlignCenter);
    m_loadingLabel->setStyleSheet("color: orange; font-weight: bold;");
    m_loadingLabel->hide();

    m_progressBar = new QProgressBar(this);

    m_deleteButton->setEnabled(false);
    m_listView = new QListView(this);
    m_listView->setItemDelegate(new TaskDelegate(this));

    QVBoxLayout* layout = new QVBoxLayout();

    m_settings = new QSettings("NicoDev", "ToDoList", this);

    layout->addWidget(m_lineEdit);
    layout->addWidget(m_pushButton);
    layout->addWidget(m_listView);
    layout->addWidget(m_saveButton);
    layout->addWidget(m_loadButton);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_ascendingButton);
    layout->addWidget(m_descendingButton);
    layout->addWidget(m_settingsButton);
    layout->addWidget(m_progressBar);
    layout->addWidget(m_loadingLabel);
    centralWidget->setLayout(layout);
}

void MainWindow::setupWorkers()
{
    m_thread = new QThread(this);
    m_dataLoader = new DataLoader();

    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(5000);

    m_networkManager = new QNetworkAccessManager(this);

    m_dataLoader->moveToThread(m_thread);
}

void MainWindow::setupModel()
{
    m_taskModel = new TaskListModel(this);
    m_model = new QSortFilterProxyModel(this);
    m_listView->setModel(m_model);

    m_model->setSourceModel(m_taskModel);
    // Indiquer quel rôle utiliser pour le tri/filtre
    m_model->setFilterRole(TaskRoles::NameRole);
    m_model->setSortRole(TaskRoles::NameRole);
    m_model->sort(0, Qt::DescendingOrder);
}

void MainWindow::setupConnections()
{
    connect(m_pushButton, &QPushButton::clicked, this, &MainWindow::getLineEditText);
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::saveTasks);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedTask);
    connect(m_ascendingButton, &QPushButton::clicked, this, [this]() { filterOrder(true); });
    connect(m_descendingButton, &QPushButton::clicked, this, [this]() { filterOrder(false); });
    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged, this,
            [this](const QModelIndex current) { m_deleteButton->setEnabled(current.isValid()); });
    connect(m_taskModel, &TaskListModel::taskAdded, this,
            [this](int index, const QString& name) { statusBar()->showMessage(QString("Added: %1").arg(name)); });
    connect(m_taskModel, &TaskListModel::taskRemoved, this,
            [this](int index) { statusBar()->showMessage(QString("Removed: row number %1").arg(index)); });
    connect(m_thread, &QThread::started, m_dataLoader, &DataLoader::loadData);

    connect(m_dataLoader, &DataLoader::progressUpdated, m_progressBar, &QProgressBar::setValue);
    connect(m_dataLoader, &DataLoader::dataLoaded, this, &MainWindow::loadTasks);
    connect(m_dataLoader, &DataLoader::dataLoaded, m_thread, &QThread::quit);

    connect(m_loadButton, &QPushButton::clicked, m_thread,
            [this]()
            {
                if (m_thread->isRunning())
                {
                    return;
                }
                m_thread->start();
            });
    connect(m_settingsButton, &QPushButton::clicked, this,
            [this]
            {
                auto* dialog = new PreferenceDialog(this, *m_settings);
                connect(dialog, &PreferenceDialog::settingsChanged, this,
                        [this]() { resize(m_settings->value("window/size", QSize(800, 600)).toSize()); });
                dialog->open();
            });
}

bool MainWindow::createDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("tasks.db");
    if (!db.open())
    {
        qWarning() << "Database not open";
        return false;
    }
    QSqlQuery query;
    query.exec(
        "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "priority INTEGER DEFAULT 0,"
        "createdDate TEXT,"
        "completed INTEGER DEFAULT 0)");
    return true;
}

void MainWindow::createTaskInDatabase(TaskData& td)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO tasks (name, priority, createdDate, completed) VALUES (:name, :priority, :createdDate, "
        ":completed)");
    query.bindValue(":name", td.name);
    query.bindValue(":priority", td.priority);
    query.bindValue(":createdDate", td.createdDate.toString(Qt::ISODate));
    query.bindValue(":completed", td.completed ? 1 : 0);
    if (!query.exec())
    {
        qWarning() << "INSERT fail :" << query.lastError().text();
        return;
    }
    qWarning() << "INSERT success";
    td.id = query.lastInsertId().toInt();
}
void MainWindow::getTasksInDatabase()
{
    QSqlQuery query("SELECT id, name, priority, createdDate, completed FROM tasks");
    int count(0);
    while (query.next())
    {
        TaskData td;
        td.id = query.value("id").toInt();
        td.name = query.value("name").toString();
        td.priority = query.value("priority").toInt();
        td.createdDate = QDateTime::fromString(query.value("createdDate").toString(), Qt::ISODate);
        td.completed = query.value("completed").toBool();
        m_taskModel->addTask(td);
    }
}
void MainWindow::updateTasksInDatabase()
{
    QSqlQuery query;
    query.prepare(
        "UPDATE tasks SET name = :name, priority = :priority, createdDate = :createdDate, completed = :completed WHERE "
        "id = :id");
    auto& tasks = m_taskModel->getTasks();
    for (const TaskData& td : tasks)
    {
        query.bindValue(":id", td.id);
        query.bindValue(":name", td.name);
        query.bindValue(":priority", td.priority);
        query.bindValue(":createdDate", td.createdDate);
        query.bindValue(":completed", td.completed ? 1 : 0);

        if (!query.exec())
        {
            qWarning() << "UPDATE fail : " << query.lastError().text();
        }
        qWarning() << "UPDATE success";
    }
}
void MainWindow::deleteTasksInDatabase()
{
    QModelIndex selectedIndex = m_listView->currentIndex();
    if (!selectedIndex.isValid())
        return;
    QModelIndex source = m_model->mapToSource(selectedIndex);
    TaskData& td = m_taskModel->getTasks()[source.row()];

    if (td.id == -1)
    {
        m_taskModel->removeRows(source.row(), 1);
        return;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM tasks WHERE id = :id");
    query.bindValue(":id", td.id);

    if (!query.exec())
    {
        qWarning() << "DELETE fail :" << query.lastError().text();
        return;
    }
    qWarning() << "DELETE success";
    m_taskModel->removeRows(source.row(), 1);
}

void MainWindow::fetchTasksFromServer()
{
    QNetworkRequest request(QUrl(QString("https://jsonplaceholder.typicode.com/todos")));
    m_loadingLabel->show();
    m_timeoutTimer->start();
    QNetworkReply* reply = m_networkManager->get(request);

    // on créer un observer pour vérifier que le pointer existe encore dans le cas ou les requêtes partirais de manière
    // trop proche
    QPointer<QNetworkReply> safeReply = reply;

    // évite multiple connection avec des reply qui n'existe plus
    m_timeoutTimer->disconnect();

    connect(reply, &QNetworkReply::finished, this,
            [this, reply]()
            {
                m_timeoutTimer->stop();
                parseReply(reply);
                reply->deleteLater();
            });

    connect(m_timeoutTimer, &QTimer::timeout, this,
            [this, safeReply]()
            {
                if (safeReply)
                {
                    safeReply->abort();
                    m_loadingLabel->setText("Timeout — serveur trop long");
                }
            });
}

void MainWindow::parseReply(QNetworkReply* reply)
{
    if (reply->error() != 0)
    {
        qWarning() << "Parsing Error";
        return;
    }
    m_loadingLabel->hide();
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray dataArray = doc.array();
    for (QJsonValue value : dataArray)
    {
        QJsonObject obj = value.toObject();
        QString title = obj["title"].toString();
        TaskData td;
        td.name = title;
        td.priority = 1;
        td.completed = obj["completed"].toBool();
        m_taskModel->addTask(td);
    }
}

void MainWindow::getLineEditText()
{
    if (m_lineEdit->text() == "")
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
    m_taskModel->saveTasksToJson();
    auto& tasks = m_taskModel->getTasks();
    for (TaskData& td : tasks)
    {
        if (td.id == -1)
            createTaskInDatabase(td);
        else
        {
            updateTasksInDatabase();
        }
    }
    XMLParser::saveTasks(tasks);
}

void MainWindow::loadTasks()
{
    m_taskModel->removeRows(0, m_taskModel->rowCount());
    // m_taskModel->loadTasksFromJson();
    // getTasksInDatabase();
    auto list = XMLParser::streamLoad();
    for (auto task : list)
    {
        m_taskModel->addTask(task);
    }
}

void MainWindow::deleteSelectedTask()
{
    QModelIndex selectedIndex = m_listView->currentIndex();
    if (selectedIndex.isValid())
    {
        // on map l'index proxy avec le vrai index du model de référence
        deleteTasksInDatabase();
        m_deleteButton->setEnabled(m_listView->currentIndex().isValid());
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
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
    if (ascending)
    {
        m_model->sort(0, Qt::AscendingOrder);
    }
    else
    {
        m_model->sort(0, Qt::DescendingOrder);
    }
}
