#include "Exception.h"

#include <QDebug>

Exception::Exception( const QString &message ) :
    _message( message )
{
    qDebug() << "Exception thrown:" << message;
}

QString Exception::message() const {
    return _message;
}
