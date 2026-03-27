#include "../include/MainWindow.hpp"
#include <QVBoxLayout>

MainWindow::MainWindow()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    this->setWindowTitle(QString("Todo List"));

    m_lineEdit = new QLineEdit(this);
    m_pushButton = new QPushButton(QString("Ajouter"),this);
    m_listWidget = new QListWidget(this);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_pushButton);
    layout->addWidget(m_listWidget);
    centralWidget->setLayout(layout);

    connect(m_pushButton, &QPushButton::clicked,this, &MainWindow::getLineEditText);
    
}

MainWindow::~MainWindow()
{
    
}

