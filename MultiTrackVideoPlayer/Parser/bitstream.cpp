#include "bitstream.h"
/**
 * @brief Bitstream::Bitstream
 */
Bitstream::Bitstream() : _file()
{
    _bitBuffer = 0;
    _bitsInBuffer = 0;
    _bytesRead = 0;
}
/**
 * @brief Bitstream::Bitstream
 * @param file
 */
Bitstream::Bitstream(QFile *file) : _file(file)
{
    _bitBuffer = 0;
    _bitsInBuffer = 0;
    _bytesRead = 0;
}
/**
 * @brief Bitstream::getBitsInBuffer
 * @return
 */
int Bitstream::getBitsInBuffer()
{
   return _bitsInBuffer;
}
/**
 * @brief Bitstream::getBytesRead
 * @return
 */
int Bitstream::getBytesRead()
{
    return _bytesRead;
}
/**
 * @brief Bitstream::nextBits
 * @param nBits
 * @return
 */
quint32 Bitstream::nextBits(int nBits)
{
    int n = nBits;
    assert(0 <= nBits && nBits <= 32);
    while (_bitsInBuffer < nBits)
    {
        assert(_bitsInBuffer <= 24);
        char c = '0';
        if (!_file->getChar(&c) || _file->atEnd())
        {
            break;
        }
        else
        {
            _bytesRead++;
            _bitBuffer <<= 8;
            _bitBuffer |= (quint32)(0xFF & (int)c);
            _bitsInBuffer += 8;
        }
    }
    return _bitBuffer >> (_bitsInBuffer - n);
}
/**
 * @brief Bitstream::readBits
 * @param nBits
 * @return
 */
quint32 Bitstream::readBits(int nBits)
{
    quint32 result = nextBits(nBits);
    _bitsInBuffer -= nBits;
    _bitBuffer &= (1UL << _bitsInBuffer) - 1; // mask out leading bits
    return result;
}
/**
 * @brief Bitstream::getByteOffset
 * @return
 */
int Bitstream::getByteOffset()
{
    return _bytesRead - _bitsInBuffer / 8;
}

bool Bitstream::eof()
{
    return _file->atEnd();
}
