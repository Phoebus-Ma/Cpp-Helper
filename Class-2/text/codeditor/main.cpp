/**
 * main.cpp
 *
 * Description:
 *      This module defines code editor.
 *
 * Reference:
 *      [qt5base/example/widgets/richtext/syntaxhighlighter].
 */

#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;

    window.resize(640, 512);
    window.show();

    return app.exec();
}
