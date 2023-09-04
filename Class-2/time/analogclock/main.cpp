/**
 * main.cpp
 *
 * Description:
 *      This module defines analog clock.
 *
 * Reference:
 *      [qt5base/example/widgets/widgets/analogclock].
 */

#include <QApplication>

#include "analogclock.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AnalogClock clock;

    clock.show();

    return app.exec();
}
