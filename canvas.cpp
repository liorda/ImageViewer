#include "canvas.h"
#include <QPainter>
#include <QPaintEvent>

Canvas::Canvas()
{
}

Canvas::~Canvas()
{
}

QPoint Canvas::Origin() const
{
    return xform.map(QPoint(0,0));
}

void Canvas::SetPixmap(const QPixmap & pix)
{
    pixmap = pix;
    QLabel::setPixmap(pix);
}

void Canvas::paintEvent(QPaintEvent *e)
{
    QPainter painter;

    painter.begin(this);
    painter.setClipRect(e->rect());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setBackgroundMode(Qt::TransparentMode);

    painter.setTransform(xform);

    painter.drawPixmap(QPointF(0, 0), pixmap);
}

void Canvas::resizeEvent(QResizeEvent* e)
{
    QLabel::resizeEvent(e);
}

