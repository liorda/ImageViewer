#ifndef CANVAS
#define CANVAS

#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class Canvas : public QLabel
{
public:
    Canvas();
    virtual ~Canvas();

    void Transform(const QTransform& xform_) { xform = xform_; update(); }
    const QTransform& Transform() const { return xform; }
    QPoint Origin() const;

public:
    void SetPixmap(const QPixmap &);
    const QPixmap& GetPixmap() const { return pixmap; }

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private:
    QPixmap pixmap;
    QTransform xform;
};

#endif // CANVAS

