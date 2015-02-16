#include "Backend/PriorityQueue.h"
#include "VideoData/videoimage.h"

PriorityQueue::PriorityQueue(QObject *parent) : QObject(parent), _queue()
{

}

PriorityQueue::~PriorityQueue()
{
    clear();
}

void PriorityQueue::push(ListOfVideoImagesPtr gop)
{
    bool wasEmpty = _queue.empty();
    foreach ( VideoImagePtr vi, *gop ) {
        //qDebug() << "**** queue:" << vi->toString();
        _queue.push( vi );
    }

    if ( wasEmpty )
        emit nonEmpty();
}

VideoImagePtr PriorityQueue::pop()
{
    VideoImagePtr vi = _queue.top();
    _queue.pop();
    return vi;
}

VideoImagePtr PriorityQueue::peek()
{
    return _queue.top();
}

void PriorityQueue::clear()
{
    _queue = std::priority_queue<VideoImagePtr, std::vector<VideoImagePtr>, Comparator>();
}

bool PriorityQueue::isEmpty() const
{
    return _queue.empty();
}

int PriorityQueue::size() const {
    return _queue.size();
}
