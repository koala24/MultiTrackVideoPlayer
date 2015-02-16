#ifndef HELPERS_TEMPLATE_H
#define HELPERS_TEMPLATE_H

#include <QList>
#include <QLinkedList>

/**
 * Deep Copy Function fuer eine QList mit Pointern.
 */
template <typename T>
QList<T*> createDeepCopyOfQList(QList<T*> source)
{
    QList<T*> destination = QList<T*>();

    for (typename  QList<T*>::const_iterator i = source.constBegin(); i != source.constEnd(); ++i)
    {
        destination.append((*i));
    }

    return destination;
}

#endif // HELPERS_TEMPLATE_H
