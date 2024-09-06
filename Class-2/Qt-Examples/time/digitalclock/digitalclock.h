/**
 * digitalclock.h
 * 
 * Description:
 *      This module implements digital clock header file.
*/

#ifndef __DIGITALCLOCK_H__
#define __DIGITALCLOCK_H__

#include <QLCDNumber>

class DigitalClock : public QLCDNumber
{
    Q_OBJECT

public:
    DigitalClock(QWidget *parent = nullptr);

private slots:
    void showTime();
};

#endif /* __DIGITALCLOCK_H__ */
