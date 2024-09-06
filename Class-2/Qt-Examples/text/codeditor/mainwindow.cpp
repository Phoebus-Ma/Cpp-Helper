/**
 * mainwindow.cpp
 *
 * Description:
 *      This module implements code editor.
 */

#include <QtWidgets>
#include <QDebug>

#include "mainwindow.h"

/**
 * MainWindow - MainWindow constructor.
 */
MainWindow::
MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupFileMenu();
    setupViewMenu();
    setupHelpMenu();
    setupEditor();

    setCentralWidget(editor);
    setWindowTitle(tr("Code Editor"));
}

/**
 * about - About software information.
 */
void MainWindow::
about()
{
    QMessageBox::about(
        this, tr("About Code editor."),
        tr("<p> <b>Code Editor</b> Version 0.1.</p>")
    );
}

/**
 * setDark - Set text edit bakground dark theme.
 */
void MainWindow::
setDark()
{
    editor->setStyleSheet("QTextEdit{background-color:rgba(211,211,211,255);}");
}

/**
 * setLight - Set text edit bakground light theme.
 */
void MainWindow::
setLight()
{
    editor->setStyleSheet("QTextEdit{background-color:rgba(255,255,255,255);}");
}

/**
 * newFile - Create new file.
 */
void MainWindow::
newFile()
{
    editor->clear();
}

/**
 * openFile - Open the file.
 */
void MainWindow::
openFile(const QString &path)
{
    fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "Code Files (*.*)");

    if (!fileName.isEmpty())
    {
        QFile file(fileName);

        if (file.open(QFile::ReadWrite | QFile::Text))
        {
            editor->setPlainText(file.readAll());

            // Set file name to title.
            setWindowTitle(file.fileName());
        }
    }
}

/**
 * fileSave - Save file.
 */
bool MainWindow::
fileSave()
{
    return true;
}

/**
 * fileSave - Save the file to the specified file name.
 */
bool MainWindow::
fileSaveAs()
{
    return true;
}

/**
 * setupEditor - Setup editor.
 */
void MainWindow::
setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = new QTextEdit;
    editor->setFont(font);

    highlighter = new Highlighter(editor->document());
}

/**
 * setupFileMenu - Setup file menu.
 */
void MainWindow::
setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), this,
            &MainWindow::newFile, QKeySequence::New);

    fileMenu->addAction(
        tr("&Open..."),
        this,
        [this]()
        { openFile(); },
        QKeySequence::Open
    );

    fileMenu->addAction(tr("&Save"), this,
            &MainWindow::fileSave, QKeySequence::Save);

    fileMenu->addAction(tr("&Save as ..."), this,
            &MainWindow::fileSaveAs);

    fileMenu->addAction(tr("E&xit"), qApp,
            &QApplication::quit, QKeySequence::Quit);
}

/**
 * setupViewMenu - Setup view menu.
 */
void MainWindow::
setupViewMenu()
{
    QMenu *viewMenu = new QMenu(tr("&View"), this);
    menuBar()->addMenu(viewMenu);

    viewMenu->addAction(tr("&Dark"), this, &MainWindow::setDark);
    viewMenu->addAction(tr("&Light"), this, &MainWindow::setLight);
}

/**
 * setupHelpMenu - Setup help menu.
 */
void MainWindow::
setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(tr("&About"), this, &MainWindow::about);
}
