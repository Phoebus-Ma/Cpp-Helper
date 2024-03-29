/**
 * main.cpp
 *
 * Description:
 *      This module defines base calendar.
 */

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;

    window.resize(640, 256);
    window.show();

    return app.exec();
}
