/**
 * mainwindow.
 *
 * Description:
 *      This module implements code editor.
 */

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include "highlighter.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

public slots:
    void about();
    void setDark();
    void setLight();
    void newFile();
    void openFile(const QString &path = QString());
    bool fileSave();
    bool fileSaveAs();

private:
    void setupEditor();
    void setupFileMenu();
    void setupViewMenu();
    void setupHelpMenu();

    QTextEdit *editor;
    QString fileName;
    Highlighter *highlighter;
};

#endif /* __MAINWINDOW_H__ */
