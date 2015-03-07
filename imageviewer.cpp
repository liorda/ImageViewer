/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "imageviewer.h"
#include "ibf.h"

//! [0]
ImageViewer::ImageViewer()
{
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    setCentralWidget(scrollArea);

    createActions();
    createMenus();
    createToolBars();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

//! [0]
//! [2]
/*
bool ImageViewer::loadFile(const QString &fileName)
{
    QImage image(fileName);
    if (image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
        imageLabel->setPixmap(QPixmap());
        imageLabel->adjustSize();
        return false;
    }
//! [2] //! [3]
    imageLabel->setPixmap(QPixmap::fromImage(image));
//! [3] //! [4]
    scaleFactor = 1.0;

    //printAct->setEnabled(true);
    //fitToWindowAct->setEnabled(true);
    updateActions();

    //if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();

    setWindowFilePath(fileName);
    return true;
}
*/

bool ImageViewer::loadFile(const QString& filename)
{
    try {
        ImageViewerNS::IBF ibf(filename);
        QPixmap pixmap;
        ibf.ToPixmap(&pixmap);
        imageLabel->setPixmap(pixmap);
        imageLabel->adjustSize();
        setWindowFilePath(ibf.name());
/*
        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                     tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
            setWindowFilePath(QString());
            imageLabel->setPixmap(QPixmap());
            imageLabel->adjustSize();
            return false;
        }
    //! [2] //! [3]
        imageLabel->setPixmap(QPixmap::fromImage(image));
    //! [3] //! [4]
        scaleFactor = 1.0;

        //printAct->setEnabled(true);
        //fitToWindowAct->setEnabled(true);
        updateActions();

        //if (!fitToWindowAct->isChecked())
            imageLabel->adjustSize();

        setWindowFilePath(fileName);
        */
        return true;
    }
    catch (...) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(filename)));
        return false;
    }

}

//! [4]

//! [2]

//! [1]
void ImageViewer::open()
{
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("IBF files (*.ibf)"));

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}
//! [1]

void ImageViewer::encode()
{
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();

    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");

    if (dialog.exec() == QDialog::Accepted) {
        QString selected = dialog.selectedFiles().first();
        QImage image(selected);
        bool ok;
        QFileInfo selectedTargetInfo(selected);
        QString target = selectedTargetInfo.baseName();
        QString name = QInputDialog::getText(this, tr("IBF Viewer"), tr("Image name"), QLineEdit::Normal, target, &ok);
        if (ok) {
            QMatrix4x4 xform; // identity by default
            ImageViewerNS::IBF ibf(image, name, xform);
            QString output_file_location = QFileDialog::getSaveFileName(this, tr("Select target"), selectedTargetInfo.baseName(), tr("IBF files (*.ibf)"));
            if (ibf.save(output_file_location))
                loadFile(output_file_location);
        }
    }
}

//! [5]
/*
void ImageViewer::print()
//! [5] //! [6]
{
    Q_ASSERT(imageLabel->pixmap());
}
//! [8]

//! [9]
void ImageViewer::zoomIn()
//! [9] //! [10]
{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}

//! [10] //! [11]
void ImageViewer::normalSize()
//! [11] //! [12]
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}
//! [12]

//! [13]
void ImageViewer::fitToWindow()
//! [13] //! [14]
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        normalSize();
    }
    updateActions();
}
//! [14]
*/

//! [15]
void ImageViewer::about()
//! [15] //! [16]
{
    QMessageBox::about(this, tr("About Image Viewer"),
            tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
               "and QScrollArea to display an image. QLabel is typically used "
               "for displaying a text, but it can also display an image. "
               "QScrollArea provides a scrolling view around another widget. "
               "If the child widget exceeds the size of the frame, QScrollArea "
               "automatically provides scroll bars. </p><p>The example "
               "demonstrates how QLabel's ability to scale its contents "
               "(QLabel::scaledContents), and QScrollArea's ability to "
               "automatically resize its contents "
               "(QScrollArea::widgetResizable), can be used to implement "
               "zooming and scaling features. </p><p>In addition the example "
               "shows how to use QPainter to print an image.</p>"));
}
//! [16]

void ImageViewer::setXFormTranslation()
{}
void ImageViewer::setXFormRotation()
{}
void ImageViewer::setXFormScale()
{}

//! [17]
void ImageViewer::createActions()
//! [17] //! [18]
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    encodeAct = new QAction(tr("&Encode..."), this);
    encodeAct->setShortcut(tr("Ctrl+E"));
    connect(encodeAct, SIGNAL(triggered()), this, SLOT(encode()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    /*
    printAct = new QAction(tr("&Print..."), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setEnabled(false);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(tr("&Fit to Window"), this);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));
    */

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // image controls
    translateAct = new QAction(tr("Translate"), this);
    translateAct->setIcon(QPixmap(":/images/transform-move-icon.png"));
    translateAct->setStatusTip(tr("Translate the image"));
    translateAct->setCheckable(true);
    connect(translateAct, SIGNAL(triggered()), this, SLOT(setXFormTranslation()));

    rotateAct = new QAction(tr("Rotate"), this);
    rotateAct->setIcon(QPixmap(":/images/transform-rotate-icon.png"));
    rotateAct->setStatusTip(tr("Rotate the image"));
    rotateAct->setCheckable(true);
    connect(rotateAct, SIGNAL(triggered()), this, SLOT(setXFormRotation()));

    scaleAct = new QAction(tr("Scale"), this);
    scaleAct->setIcon(QPixmap(":/images/transform-scale-icon.png"));
    scaleAct->setStatusTip(tr("Scale the image"));
    scaleAct->setCheckable(true);
    connect(scaleAct, SIGNAL(triggered()), this, SLOT(setXFormScale()));
}
//! [18]

//! [19]
void ImageViewer::createMenus()
//! [19] //! [20]
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(encodeAct);
    //fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    /*
    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWindowAct);
    */

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    //menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}
//! [20]

void ImageViewer::createToolBars()
{
    xformToolBar = new QToolBar(tr("Transformation"), this);
    xformActGroup = new QActionGroup(xformToolBar);
    xformToolBar->addAction(translateAct);
    xformToolBar->addAction(rotateAct);
    xformToolBar->addAction(scaleAct);
    xformActGroup->addAction(translateAct);
    xformActGroup->addAction(rotateAct);
    xformActGroup->addAction(scaleAct);
    translateAct->setChecked(true);

    addToolBar(xformToolBar);
}

//! [21]
void ImageViewer::updateActions()
//! [21] //! [22]
{
    //zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    //zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    //normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}
//! [22]
/*
//! [23]
void ImageViewer::scaleImage(double factor)
//! [23] //! [24]
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}
//! [24]
*/
//! [25]
void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
//! [25] //! [26]
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
//! [26]
