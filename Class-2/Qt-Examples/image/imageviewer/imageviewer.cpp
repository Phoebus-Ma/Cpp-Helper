/**
 * imageviewer.cpp
 * 
 * Reference Qt base example.
 */

#include "imageviewer.h"

#include <QApplication>
#include <QColorSpace>
#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QStandardPaths>
#include <QStatusBar>


/**
 * ImageViewer - ImageViewer constructor.
 */
ImageViewer::
ImageViewer(QWidget *parent)
    : QMainWindow(parent), imageLabel(new QLabel), scrollArea(new QScrollArea)
{
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);

    /* Sets the given widget to be the main window's central widget. */
    setCentralWidget(scrollArea);

    createActions();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

/**
 * loadFile - Load file.
 */
bool ImageViewer::
loadFile(const QString &fileName)
{
    QImageReader reader(fileName);

    reader.setAutoTransform(true);
    const QImage newImage = reader.read();

    if (newImage.isNull())
    {
        QMessageBox::information(
            this,
            QGuiApplication::applicationDisplayName(),
            tr("Cannot load %1: %2")
                .arg(
                    QDir::toNativeSeparators(fileName),
                    reader.errorString()
                )
        );

        return false;
    }

    setImage(newImage);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
                                .arg(QDir::toNativeSeparators(fileName))
                                .arg(image.width())
                                .arg(image.height())
                                .arg(image.depth());

    statusBar()->showMessage(message);

    return true;
}

/**
 * setImage - Display image.
 */
void ImageViewer::
setImage(const QImage &newImage)
{
    image = newImage;

    if (image.colorSpace().isValid())
        image.convertToColorSpace(QColorSpace::SRgb);

    imageLabel->setPixmap(QPixmap::fromImage(image));

    scaleFactor = 1.0;

    scrollArea->setVisible(true);
    fitToWindowAct->setEnabled(true);

    updateActions();

    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();
}

/**
 * saveFile - Save image file.
 */
bool ImageViewer::
saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(image))
    {
        QMessageBox::information(
            this,
            QGuiApplication::applicationDisplayName(),
            tr("Cannot write %1: %2")
                .arg(QDir::toNativeSeparators(fileName)),
            writer.errorString()
        );

        return false;
    }

    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);

    return true;
}

/**
 * initializeImageFileDialog - Initialize image FileDialog.
 */
static void
initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog)
    {
        firstDialog = false;
        const QStringList picturesLocations =
            QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);

        dialog.setDirectory(picturesLocations.isEmpty()
            ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;

    const QByteArrayList supportedMimeTypes =
            acceptMode == QFileDialog::AcceptOpen
            ? QImageReader::supportedMimeTypes()
            : QImageWriter::supportedMimeTypes();

    for (const QByteArray &mimeTypeName : supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);

    mimeTypeFilters.sort();

    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    dialog.setAcceptMode(acceptMode);

    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

/**
 * open - Open file location.
 */
void ImageViewer::
open()
{
    QFileDialog dialog(this, tr("Open File"));

    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted &&
        !loadFile(dialog.selectedFiles().constFirst()))
    {
    }
}

/**
 * saveAs - Save the file as.
 */
void ImageViewer::
saveAs()
{
    QFileDialog dialog(this, tr("Save File As"));

    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted &&
        !saveFile(dialog.selectedFiles().constFirst()))
    {
    }
}

/**
 * zoomIn - Zoom in image.
 */
void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}

/**
 * zoomOut - Zoom out image.
 */
void ImageViewer::
zoomOut()
{
    scaleImage(0.8);
}

/**
 * normalSize - Set image to normal size.
 */
void ImageViewer::
normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

/**
 * fitToWindow - Adjust the picture to fit the window.
 */
void ImageViewer::
fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();

    scrollArea->setWidgetResizable(fitToWindow);

    if (!fitToWindow)
        normalSize();

    updateActions();
}

/**
 * about - About software information.
 */
void ImageViewer::
about()
{
    QMessageBox::about(
        this,
        tr("About Image Viewer"),
        tr("<p>Image wiewer version v0.1</p>")
    );
}

/**
 * createActions - Create application menubar.
 */
void ImageViewer::
createActions()
{
    /* 1. File menubar. */
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &ImageViewer::open);
    openAct->setShortcut(QKeySequence::Open);

    saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &ImageViewer::saveAs);
    saveAsAct->setEnabled(false);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    /* 2. View menubar. */
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ImageViewer::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ImageViewer::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ImageViewer::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+R"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &ImageViewer::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));

    /* 3. Help menubar */
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    helpMenu->addAction(tr("&About"), this, &ImageViewer::about);
}

/**
 * updateActions - Update actions.
 */
void ImageViewer::
updateActions()
{
    saveAsAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

/**
 * scaleImage - Scale image.
 */
void ImageViewer::
scaleImage(double factor)
{
    scaleFactor *= factor;

    imageLabel->resize(scaleFactor * imageLabel->pixmap(Qt::ReturnByValue).size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

/**
 * adjustScrollBar - Adjust scrollbar.
 */
void ImageViewer::
adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() +
            ((factor - 1) * scrollBar->pageStep() / 2)));
}
