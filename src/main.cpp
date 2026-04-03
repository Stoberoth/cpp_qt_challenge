#include <QApplication>
#include <QDebug>
#include <iostream>

#include "../include/MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow = MainWindow();
    mainWindow.show();

    return app.exec();
}
