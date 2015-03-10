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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QFuture>
#include <QFutureWatcher>

#include "ibf.h"
#include "canvas.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QToolButton;
class QActionGroup;
QT_END_NAMESPACE

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer();
    bool loadFile(const QString &);
    bool loadFile(const IBF& ibf);
    static IBF* LoadFromDisk(const QString& filename);

private:
    QFutureWatcher<IBF*> ibfLoadedFutureWatcher;

private:
    bool saveFile(const QImage& image, const QMatrix4x4& xform, const QString& name, const QString& targetFilename);    

private slots:
    void load();
    void saveAs();
    void encode();
    void about();

    Q_SLOT void ibLoadFinished();

    void setXFormTranslation();
    void setXFormRotation();
    void setXFormScale();
    void resetTransformations();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void createActions();
    void createMenus();
    void createToolBars();

    Canvas *canvas;

    QAction *loadAct;
    QAction *encodeAct;
    QAction *saveAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QActionGroup *xformActGroup;
    QAction *translateAct;
    QAction *rotateAct;
    QAction *scaleAct;
    QAction *resetXFormAct;

    QMenu *fileMenu;
    QMenu *helpMenu;

    QToolBar* xformToolBar;

    QString loadedFile;

    enum XFromControl { INVALID = 0, TRANSLATE, ROTATE, SCALE };
    enum XFromControl xformControl;

    QTransform xformPending;

    QPoint lastMousePos; // constantly updating
    QPoint firstMousePos; // updated only on starting drag
};

#endif
