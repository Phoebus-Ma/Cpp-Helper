/**
 * main.cpp
 *
 * Description:
 *      This module defines digital clock.
 * 
 * Reference:
 *      [qt5base/example/widgets/widgets/digitalclock].
 */


#include <QApplication>

#include "digitalclock.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DigitalClock clock;

    clock.show();

    return app.exec();
}
