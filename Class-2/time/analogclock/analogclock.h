/**
 * analogclock.h
 *
 * Description:
 *      This module implement analog clock header file.
 */

#ifndef __ANALOGCLOCK_H__
#define __ANALOGCLOCK_H__

#include <QWidget>


class AnalogClock : public QWidget
{
    Q_OBJECT

public:
    AnalogClock(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};


#endif /* __ANALOGCLOCK_H__ */
