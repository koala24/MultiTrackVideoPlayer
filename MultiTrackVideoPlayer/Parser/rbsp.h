#ifndef RBSP_H
#define RBSP_H

#include <QtGlobal>
#include <QFile>
#include "bitstream.h"
#include "offsettype.h"

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

class Rbsp // raw byte sequence payload
{
private:
    const int _rbspSize;
    uint8_t *_rbsp;
    int _bytesInBuffer;
    int _bytesRead;
    int _bitsRead;
    int _bitsInBuffer;
    uint32_t _bitBuffer;

public:
    Rbsp();
    virtual ~Rbsp();

    void appendByte(uint8_t byte);
    uint32_t nextBits(int nBits);
    uint32_t readBits(int nBits);
    int bitsInRbsp();
    bool moreRbspData();

protected:
    int getByte();
};

#endif // RBSP_H
