#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <QtGlobal>
#include <QFile>
#include <assert.h>

class Bitstream // Annex B stream (byte oriented)
{
private:
    quint32 _bitBuffer;
    int _bitsInBuffer;
    int _bytesRead;
    QFile *_file;

public:
    Bitstream();
    Bitstream(QFile *file);

    int getBitsInBuffer();
    int getBytesRead();

    quint32 nextBits(int nBits);
    quint32 readBits(int nBits);
    int getByteOffset();
    bool eof();
};

#endif // BITSTREAM_H
