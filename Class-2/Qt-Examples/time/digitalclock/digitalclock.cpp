/**
 * digitalclock.cpp
 *
 * Description:
 *      This module implements digital clock.
 */

#include "digitalclock.h"

#include <QTime>
#include <QTimer>


/**
 * DigitalClock - DigitalClock constructor.
 */
DigitalClock::
DigitalClock(QWidget *parent) : QLCDNumber(parent)
{
    // LCD number style.
    setSegmentStyle(Filled);

    // Create timer.
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DigitalClock::showTime);

    // Starts a timer that fires every 1000 milliseconds.
    timer->start(1000);

    // Display time.
    showTime();

    setWindowTitle(tr("Digital Clock"));
    resize(150, 60);
}

/**
 * showTime - Display time.
 */
void DigitalClock::
showTime()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm");

    /**
     * hh:mm
     * text[1] - hh
     * text[2] - :
     * text[3] - mm
     * 
     * The ":" sign and space alternate every second.
     */
    if ((time.second() % 2) == 0)
        text[2] = ' ';

    display(text);
}
