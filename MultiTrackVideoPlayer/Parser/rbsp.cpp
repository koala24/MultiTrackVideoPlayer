#include "rbsp.h"
/**
 * @brief Rbsp::Rbsp
 */
Rbsp::Rbsp() : _rbspSize(10000000)
{
    _bytesInBuffer = 0;
    _bytesRead = 0;
    _bitsRead = 0;
    _bitsInBuffer = 0;
    _bitBuffer = 0;
    _rbsp = new quint8[_rbspSize];
}
/**
 * @brief Rbsp::~Rbsp
 */
Rbsp::~Rbsp()
{
    delete[] _rbsp;
}
/**
 * @brief Rbsp::appendByte
 * @param byte
 */
// rbsp must be filled by calling appendByte first
// once next_bits or read_bits was called, no more bytes must be appended
void Rbsp::appendByte(quint8 byte)
{
    assert(_bytesInBuffer < _rbspSize); // this is lean but mean!
    _rbsp[_bytesInBuffer++] = byte;
}
/**
 * @brief Rbsp::anextBits
 * @param nBits
 * @return
 */
quint32 Rbsp::nextBits(int nBits)
{
    int n = nBits;
    assert(0 <= nBits && nBits <= 32);
    while (_bitsInBuffer < nBits)
    {
        assert(_bitsInBuffer <= 24);
        int byte = getByte();
        if (byte != -1)
        {
            nBits = 0;
        }
        else
        {
            _bitBuffer <<= 8;
            _bitBuffer |= (quint32)(0xFF & byte);
            _bitsInBuffer += 8;
        }
    }
    return _bitBuffer >> (_bitsInBuffer - n);
}
/**
 * @brief Rbsp::areadBits
 * @param nBits
 * @return
 */
quint32 Rbsp::readBits(int nBits)
{
    quint32 result = nextBits(nBits);
    _bitsInBuffer -= nBits;
    _bitBuffer &= (1UL << _bitsInBuffer) - 1; // mask out leading bits
    _bitsRead += nBits;
    return result;
}
/**
 * @brief Rbsp::bitsInRbsp
 * @return
 */
int Rbsp::bitsInRbsp()
{
    int result = 8 * _bytesInBuffer;
    quint8 lastByte = _rbsp[_bytesInBuffer - 1];
    assert(lastByte != 0x00);
    while (!(lastByte & 1))
    {
        result--;
        lastByte >>= 1;
    }
    result--; // trailing 1 bit
    return result;
}
/**
 * @brief Rbsp::moreRbspData
 * @return
 */
bool Rbsp::moreRbspData()
{
    return _bitsRead < bitsInRbsp();
}
/**
 * @brief Rbsp::getByte
 * @return
 */
int Rbsp::getByte() {
    if (_bytesRead < _bytesInBuffer)
    {
        return _rbsp[_bytesRead++];
    }
    else
    {
        return -1;
    }
}
