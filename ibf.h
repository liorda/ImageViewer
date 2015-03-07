#ifndef IBF_H
#define IBF_H

#include <QMainWindow>
#include <QMatrix4x4>

QT_BEGIN_NAMESPACE
class QString;
class QByteArray;
QT_END_NAMESPACE


/*
A very simple and limited image file format.
Specification:
+--------------+-------------+--------------------------+--------------------------------+
| offset       | length      | description              | hardcoded values for version 1 |
+--------------+-------------+--------------------------+--------------------------------+
| 0            | 3           | Magic id: 'IBF'          |                                |
+--------------+-------------+--------------------------+--------------------------------+
| 3            | 1           | Version                  | 0x1                            |
+--------------+-------------+--------------------------+--------------------------------+
| 4            | 4           | header length (L)        | 89+N                           |
+--------------+-------------+--------------------------+--------------------------------+
| 8            | 4           | name length (N)          |                                |
+--------------+-------------+--------------------------+--------------------------------+
| 12           | N           | name (unicode)           |                                |
+--------------+-------------+--------------------------+--------------------------------+
| 12+N         | 1           | format id:               | 0x1                            |
|              |             | 0x0: 8 graylevel bits    |                                |
|              |             | 0x1: R8G8B8A8            |                                |
|              |             | ...                      |                                |
+--------------+-------------+--------------------------+--------------------------------+
| 13+N         | 4           | image width (pixels)     |                                |
+--------------+-------------+--------------------------+--------------------------------+
| 17+N         | 4           | image height (pixels)    |                                |
+--------------+-------------+--------------------------+--------------------------------+
| 21+N         | 64          | Transformation matrix    |                                |
|              |             | (4x4 floats)             |                                |
+--------------+-------------+--------------------------+--------------------------------+
| 85+N         | 4           | pixel data length (M)    | W*H*4                          |
+--------------+-------------+--------------------------+--------------------------------+
| 89+N         | L-(89+N)    | reserved                 |                                |
+--------------+-------------+--------------------------+--------------------------------+
| L            | M           | pixel data               |                                |
+--------------+-------------+--------------------------+--------------------------------+
*/

namespace ImageViewerNS
{
class IBF
{
public:
    IBF(const QString& IBFFilename); // load an ibf from disk
    IBF(const QImage& image, const QString& name, const QMatrix4x4& xform); // encode an ibf
    ~IBF();

    bool save(const QString& targetFilename) const;
    const QString& name() const { return _name; }
    bool ToPixmap(QPixmap* pixmap) const;

private:
    bool load(const QString& IBFFilename);

private:
    unsigned char _version;
    QString _name;

    enum format {
        GRAY8 = 0,
        R8G8B8A8 = 1
    };
    enum format _format;

    int _width;
    int _height;

    QMatrix4x4 _xform;
    QByteArray _data;
};

} // namespace ImageViewerNS

#endif // IBF_H
