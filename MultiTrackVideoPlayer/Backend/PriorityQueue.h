#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <QObject>
#include <queue>
#include "VideoData/videoimage.h"
#include "Decoder/IDecoder.h"

struct Comparator {
    bool operator()( const VideoImagePtr& lhs, const VideoImagePtr& rhs ) {
        return lhs->getPlaybackTimestamp() > rhs->getPlaybackTimestamp();
    }
};

class PriorityQueue : public QObject
{
    Q_OBJECT

public:
    PriorityQueue( QObject *parent = 0 );
    ~PriorityQueue();

    void push( ListOfVideoImagesPtr gop );

    /* oberstes Element herausnehmen */
    VideoImagePtr pop();
    /* Element an elementIndex zurueckgeben, aber nicht herausnehmen */
    VideoImagePtr peek();

    void clear();
    bool isEmpty() const;

    int size() const;

signals:
    void nonEmpty(); // sobald wieder ein Element in der Liste ist

private:
    std::priority_queue<VideoImagePtr, std::vector<VideoImagePtr>, Comparator> _queue;
};

#endif // PRIORITYQUEUE_H
