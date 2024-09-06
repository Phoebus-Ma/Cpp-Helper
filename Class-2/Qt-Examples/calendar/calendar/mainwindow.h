/**
 * mainwindow.h
 *
 * Description:
 *      This file contains calendar header file.
 */

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QDate>
#include <QtWidgets>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QTextBrowser;
QT_END_NAMESPACE

#define USE_LANGUAGE_ENGLISH

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

public slots:
    void setFontSize(int size);
    void setMonth(int month);
    void setYear(QDate date);

private:
    void insertCalendar();

    int fontSize;
    QDate selectedDate;
    QLocale locale;
    QTextBrowser *editor;
};

#endif /* __MAINWINDOW_H__ */
