/**
 * analogclock.cpp
 *
 * Description:
 *      This module implement analog clock.
 */

#include "analogclock.h"

#include <QPainter>
#include <QTime>
#include <QTimer>


/**
 * AnalogClock - AnalogClock constructor.
 */
AnalogClock::
AnalogClock(QWidget *parent) : QWidget(parent)
{
    // Create timer.
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&AnalogClock::update));
    
    // Loop for 1000ms for timer.
    timer->start(1000);

    setWindowTitle(tr("Analog Clock"));
    resize(200, 200);
}

/**
 * paintEvent - Draw hour and minute hand.
 */
void AnalogClock::
paintEvent(QPaintEvent *)
{
    static const QPoint hourHand[3] = {
        QPoint(7,  8),
        QPoint(-7, 8),
        QPoint(0, -40)
    };

    static const QPoint minuteHand[3] = {
        QPoint(7,  8),
        QPoint(-7, 8),
        QPoint(0, -70)
    };

    // Set hour and minute hand colors.
    QColor hourColor(127, 0, 127);
    QColor minuteColor(0, 127, 127, 191);

    // Set analog clock display area.
    int side   = qMin(width(), height());

    // Get current time.
    QTime time = QTime::currentTime();

    QPainter painter(this);

    // Set painter property.
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);

    /* --------Step 1, Set hour.-------- */

    // Set brush, no pen.
    painter.setPen(Qt::NoPen);              // Sets the painter's pen to be the given a pen.
    painter.setBrush(hourColor);            // Sets the painter's brush to the given a brush.

    // Saves the current painter state (pushes the state onto a stack).
    painter.save();

    // Rotate the hour scale to the specified Angle, The hour line is 30 degrees per unit.
    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));

    // Draw triangle for hourHand defines parameters.
    painter.drawConvexPolygon(hourHand, 3);

    // Restores the current painter state (pops a saved state off the stack).
    painter.restore();

    painter.setPen(hourColor);

    // Set a 12-hour tick mark.
    for (int i = 0; i < 12; ++i)
    {
        painter.drawLine(88, 0, 96, 0);     // Line length: 8.
        painter.rotate(30.0);               // Rotate 30°.
    }

    /* --------Setp 2, Set minute.-------- */

    // Set brush, no pen.
    painter.setPen(Qt::NoPen);
    painter.setBrush(minuteColor);

    // Draw minute line and rotate it to the specified Angle, The minute line is 6 degress per unit.
    painter.save();
    painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();

    painter.setPen(minuteColor);

    // Set 5 * 12 minutes tick mark.
    for (int j = 0; j < 60; ++j)
    {
        if ((j % 5) != 0)
            painter.drawLine(92, 0, 96, 0); // Line length: 4.

        painter.rotate(6.0);                // Rotate 6°.
    }
}
