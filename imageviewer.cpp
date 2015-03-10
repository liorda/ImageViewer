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
#include <QtConcurrent/QtConcurrent>

#include "imageviewer.h"
#include "ibf.h"

ImageViewer::ImageViewer() : ibfLoadedFutureWatcher(this)
{
    canvas = new Canvas;
    canvas->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setCentralWidget(canvas);

    createActions();
    createMenus();
    createToolBars();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

    connect(&ibfLoadedFutureWatcher, SIGNAL(finished()), this, SLOT(ibLoadFinished()));

    statusBar()->showMessage(tr("Ready"));

    // debug mode
    //loadFile("c:\\Users\\Public\\Pictures\\Jellyfish.ibf");
}

bool ImageViewer::loadFile(const QString& filename)
{
    try {
        if (ibfLoadedFutureWatcher.isRunning())
            return false;
        QFuture<IBF*> ibfLoadedFuture = QtConcurrent::run(LoadFromDisk, filename);
        ibfLoadedFutureWatcher.setFuture(ibfLoadedFuture);
        loadedFile = filename;
        return true;
    }
    catch (...) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(filename)));
    }
    return false;
}

bool ImageViewer::loadFile(const IBF& ibf)
{
    QPixmap orig;
    if (!ibf.ToPixmap(&orig))
        return false;
    canvas->SetPixmap(orig);
    canvas->adjustSize();
    setWindowFilePath(ibf.name());
    xformPending = ibf.Transform().toTransform();
    canvas->Transform(xformPending);
    saveAct->setEnabled(true);
    xformToolBar->setEnabled(true);
    return true;
}

IBF* ImageViewer::LoadFromDisk(const QString &filename)
{
    try {
        IBF* ibf = new IBF(filename);
        return ibf;
    }
    catch (...) {
        return 0;
    }
}

void ImageViewer::ibLoadFinished()
{
    IBF* ibf = ibfLoadedFutureWatcher.result();
    loadFile(*ibf);
    delete ibf;
}

bool ImageViewer::saveFile(const QImage& image, const QMatrix4x4& xform, const QString& name, const QString& targetFilename)
{
    IBF ibf(image, name, xform);

    if (ibf.save(targetFilename))
        return loadFile(targetFilename);

    return false;
}

void ImageViewer::load()
{
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Load File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("IBF files (*.ibf)"));

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::saveAs()
{
    if (loadedFile.isNull() || loadedFile.isEmpty())
        return;

    if (canvas->GetPixmap().isNull())
        return;

    QString targetFilename = QFileDialog::getSaveFileName(this, tr("Select target"), loadedFile, tr("IBF files (*.ibf)"));
    if (targetFilename.isNull() || targetFilename.isEmpty())
        return;

    bool ok;
    QFileInfo targetInfo(loadedFile);
    QString targetName = QInputDialog::getText(this, tr("IBF Viewer"), tr("Image name"), QLineEdit::Normal, targetInfo.baseName(), &ok);
    if (!ok || targetName.isNull() || targetName.isEmpty())
        return;

    saveFile(canvas->GetPixmap().toImage(), xformPending, targetName, targetFilename);
}

void ImageViewer::encode()
{
    QMimeDatabase db;
    QString all("All image files (");
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes()) {
        QMimeType type = db.mimeTypeForName(mimeTypeName);
        const QStringList exts = type.suffixes();
        for (int i=0; i<exts.length(); ++i) {
            all.append("*." + exts[i] + " ");
        }
    }
    all = all.left(all.length()-1) + ")";

    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(all);

    if (dialog.exec() == QDialog::Accepted) {
        QString selected = dialog.selectedFiles().first();
        QImage image(selected);
        QFileInfo selectedTargetInfo(selected);
        bool ok;
        QString targetName = QInputDialog::getText(this, tr("IBF Viewer"), tr("Image name"), QLineEdit::Normal, selectedTargetInfo.baseName(), &ok);
        if (ok)
        {
            IBF ibf(image, targetName, QMatrix4x4());
            QString targetFilename = QFileDialog::getSaveFileName(this, tr("Select target"), selectedTargetInfo.canonicalPath() + "/" + selectedTargetInfo.baseName(), tr("IBF files (*.ibf)"));
            if (targetFilename.isNull() || targetFilename.isEmpty())
                return;
            if (!ibf.save(targetFilename))
                return;
            loadFile(ibf);
            loadedFile = targetFilename;
        }
    }
}

void ImageViewer::about()
{
    QMessageBox::about(this, tr("About Image Viewer"),
            tr("<p>The <b>Image Viewer</b>!</p>"));
}

void ImageViewer::setXFormTranslation()
{
    xformControl = TRANSLATE;
}
void ImageViewer::setXFormRotation()
{
    xformControl = ROTATE;
}

void ImageViewer::setXFormScale()
{
    xformControl = SCALE;
}

void ImageViewer::resetTransformations()
{
    xformPending.reset();
    canvas->Transform(xformPending);
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
    firstMousePos = event->pos();
}

void ImageViewer::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->x() - lastMousePos.x();
    int dy = e->y() - lastMousePos.y();
    float fdx = (float)dx;
    float fdy = (float)dy;

    if (e->buttons() & Qt::LeftButton) {
        QTransform x;
        switch (xformControl)
        {
        case TRANSLATE:
        {
            x.translate(fdx, fdy);

            QString msg;
            msg.sprintf("Translating (%.2f,%.2f)", fdx, fdy);
            statusBar()->showMessage(msg);

            break;
        }
        case ROTATE:
        {            
            const float fValue = fdx + fdy; // very naive

            x.rotate(fValue);

            QString msg;
            msg.sprintf("Rotating %.2f degrees", fValue);
            statusBar()->showMessage(msg);

            break;
        }
        case SCALE:
        {
            const float dW = fdx / canvas->GetPixmap().width();
            const float dH = fdy / canvas->GetPixmap().height();
            const float xScale = 1.0f + dW;
            const float yScale = 1.0f + dH;
            x.scale(xScale, yScale);

            // TODO avoid shearing

            QString msg;
            msg.sprintf("scaling (%.2f,%.2f)", xScale, yScale);
            statusBar()->showMessage(msg);

            break;
        }
        }
        xformPending = xformPending * x;
        canvas->Transform(xformPending);
    }
    lastMousePos = e->pos();
}

void ImageViewer::createActions()
{
    loadAct = new QAction(tr("&Load..."), this);
    loadAct->setShortcut(tr("Ctrl+L"));
    connect(loadAct, SIGNAL(triggered()), this, SLOT(load()));

    encodeAct = new QAction(tr("&Encode..."), this);
    encodeAct->setShortcut(tr("Ctrl+E"));
    connect(encodeAct, SIGNAL(triggered()), this, SLOT(encode()));

    saveAct = new QAction(tr("&Save as..."), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(saveAs()));
    saveAct->setEnabled(false);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // image controls
    resetXFormAct = new QAction(tr("Reset"), this);
    resetXFormAct->setIcon(QPixmap(":/images/Action Undo.png"));
    resetXFormAct->setStatusTip(tr("Reset all transformations"));
    resetXFormAct->setCheckable(false);
    connect(resetXFormAct, SIGNAL(triggered()), this, SLOT(resetTransformations()));

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

void ImageViewer::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(loadAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(encodeAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(helpMenu);
}

void ImageViewer::createToolBars()
{
    xformToolBar = new QToolBar(tr("Transformation"), this);

    xformActGroup = new QActionGroup(xformToolBar);
    xformActGroup->addAction(translateAct);
    xformActGroup->addAction(rotateAct);
    xformActGroup->addAction(scaleAct);

    xformToolBar->addAction(translateAct);
    xformToolBar->addAction(rotateAct);
    xformToolBar->addAction(scaleAct);
    xformToolBar->addSeparator();
    xformToolBar->addAction(resetXFormAct);

    translateAct->setChecked(true);
    xformControl = TRANSLATE;

    addToolBar(xformToolBar);

    xformToolBar->setEnabled(false);
}
