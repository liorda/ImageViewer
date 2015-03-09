#include "ibf.h"
#include <QFile>

IBF::IBF(const QString& IBFFilename)
{
    if (!load(IBFFilename))
        throw std::runtime_error("Failed loading file from disk!");
}

IBF::IBF(const QImage& image, const QString& name, const QMatrix4x4& xform) :
    _version(1),
    _name(name),
    _width(image.width()),
    _height(image.height()),
    _format(format::R8G8B8A8),
    _xform(xform)
{
    if (QImage::toImageFormat(image.pixelFormat()) == QImage::Format_ARGB32) {
        _data.append((const char*)(image.constBits()), image.byteCount());
    }
    else {
        QImage copy = image.convertToFormat(QImage::Format_ARGB32);
        _data.append((const char*)(copy.constBits()), copy.byteCount());
    }
}

IBF::~IBF()
{

}

bool IBF::save(const QString& targetFilename) const
{
    QFile file(targetFilename);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    // add header

    const char* magic = "IBF";
    const unsigned char ver = 0x1;
    const QByteArray name = _name.toUtf8();
    const int namelen  = name.length();
    const unsigned char format = 0x1;
    const int w = _width;
    const int h = _height;
    const int datalen = w * h * 4;
    const int headerlen = 89 + namelen;

    QByteArray arr;
    arr.resize(headerlen);
    arr[0] = magic[0];
    arr[1] = magic[1];
    arr[2] = magic[2];
    arr[3] = ver;
    arr[4] = (headerlen >> 24) & 0xFF;
    arr[5] = (headerlen >> 16) & 0xFF;
    arr[6] = (headerlen >> 8) & 0xFF;
    arr[7] = headerlen & 0xFF;
    arr[8] = (namelen >> 24) & 0xFF;
    arr[9] = (namelen >> 16) & 0xFF;
    arr[10] = (namelen >> 8) & 0xFF;
    arr[11] = namelen & 0xFF;
    for (int i=0; i<namelen; ++i)
        arr[12+i] = name[i];
    arr[12+namelen] = format;
    arr[13+namelen] = (w >> 24) & 0xFF;
    arr[14+namelen] = (w >> 16) & 0xFF;
    arr[15+namelen] = (w >> 8) & 0xFF;
    arr[16+namelen] = w & 0xFF;
    arr[17+namelen] = (h >> 24) & 0xFF;
    arr[18+namelen] = (h >> 16) & 0xFF;
    arr[19+namelen] = (h >> 8) & 0xFF;
    arr[20+namelen] = h & 0xFF;

    for (int i=0; i<16; ++i) {
        const float* rawData = _xform.constData();
        const float f = rawData[i];
        const uint n = *((uint*)(&f));
        arr[21+namelen+4*i] = (n >> 24) & 0xFF;
        arr[21+namelen+4*i+1] = (n >> 16) & 0xFF;
        arr[21+namelen+4*i+2] = (n >> 8) & 0xFF;
        arr[21+namelen+4*i+3] = n & 0xFF;
    }

    arr[85+namelen] = (datalen >> 24) & 0xFF;
    arr[86+namelen] = (datalen >> 16) & 0xFF;
    arr[87+namelen] = (datalen >> 8) & 0xFF;
    arr[88+namelen] = datalen & 0xFF;

    Q_ASSERT(w*h*4 == _data.length());
    Q_ASSERT(datalen == _data.length());
    Q_ASSERT(headerlen == arr.length());

    // write header
    const qint64 headerwritten = file.write(arr);
    if (arr.length() != headerwritten) {
        file.close();
        return false;
    }

    // write pixels
    const qint64 datawritten = file.write(_data);
    if (_data.length() != datawritten) {
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool IBF::ToPixmap(QPixmap* pixmap) const
{
    if (!pixmap)
        return false;
    QImage image((const uchar*)(_data.constData()), _width, _height, QImage::Format_ARGB32);
    *pixmap = QPixmap::fromImage(image);
    return true;
}

bool IBF::load(const QString &IBFFilename)
{
    QFile file(IBFFilename);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    unsigned char* bytes = (unsigned char*)(data.data());

    file.close();

    if (data[0] != 'I' || data[1] != 'B' || data[2] != 'F')
        return false;

    _version = bytes[3];
    if (0x1 != _version)
        return false;

    const int headerlen = (
        (bytes[4] << 24) |
        (bytes[5] << 16) |
        (bytes[6] << 8) |
        (bytes[7]) );
    const int namelen = (
        (bytes[8] << 24) |
        (bytes[9] << 16) |
        (bytes[10] << 8) |
        (bytes[11]) );

    QByteArray name;
    for (int i=0; i<namelen; ++i)
        name.push_back((char)(bytes[12+i]));
    _name = QString(name);

    _format = (format)(bytes[namelen+12]);
    if (format::R8G8B8A8 != _format)
        return false;

    _width = (
        (bytes[namelen+13] << 24) |
        (bytes[namelen+14] << 16) |
        (bytes[namelen+15] << 8) |
        (bytes[namelen+16]) );

    _height = (
        (bytes[namelen+17] << 24) |
        (bytes[namelen+18] << 16) |
        (bytes[namelen+19] << 8) |
        (bytes[namelen+20]) );

    unsigned char* xformBytes = &bytes[namelen+21];
    float* at = new float[16];
    for (int i=0; i<16; ++i) {
        const unsigned char b0 = xformBytes[4*i];
        const unsigned char b1 = xformBytes[4*i+1];
        const unsigned char b2 = xformBytes[4*i+2];
        const unsigned char b3 = xformBytes[4*i+3];
        const uint n0 = ( b0 << 24 );
        const uint n1 = ( b1 << 16 );
        const uint n2 = ( b2 << 8 );
        const uint n3 = ( b3 );
        const uint n = n0 | n1 | n2 | n3;
        const float f = *(float*)(&n);
        at[i] = f;
    }
    _xform = QMatrix4x4(at); // this c'tor is assuming row-major order
    _xform = _xform.transposed();

    delete [] at;

    const int datalen = (
        (bytes[namelen+85] << 24) |
        (bytes[namelen+86] << 16) |
        (bytes[namelen+87] << 8) |
        (bytes[namelen+88]) );

    if (datalen != _width * _height * 4)
        return false;

    _data.resize(datalen);
    for (int i=0; i<datalen; ++i)
        _data[i] = data[headerlen+i];

    return true;
}
