#ifndef TRACKBAR_H
#define TRACKBAR_H

#include <QWidget>
#include <QToolTip>
#include <QTime>
#include <QDebug>
#include <QMouseEvent>
#include <QMovie>
#include "Enums.h"

namespace Ui {
class Trackbar;
}

class Trackbar : public QWidget
{
    Q_OBJECT

private:
    int _maxTimestamp;
    int _currentTimestamp;
    QTime _currentTime;
    QTime _maxTime;
    QMovie *_loadingGif;
    int _resolution;
    bool _sliderPressed;
    PlayDirection::Direction _lastPlayDirection;
    bool _hasPlayed;
    bool _halt;

public:
    explicit Trackbar(QWidget *parent = 0);
    Trackbar(int maxTimestamp, QWidget *parent);
    ~Trackbar();

    void setMaxTimestamp(int maxTimestamp);
    int getMaxTimestamp();
    void updateTrackbarCurrentTimestamp(int currentTimestamp);
    void setBufferingMessageEnabled(bool buffering);
    void init(int maxTimestamp = 0);
    void setHalt(bool halt);
    bool isRepeatChecked();

signals:
    void timestampChanged(int);
    void stopClicked();
    void pauseClicked();
    void playForwardClicked();
    void playBackwardClicked();
    void speedChanged(qreal);
    void sliderMoved(int);
    void repeatClicked(bool);

public slots:
    void jumpToTimestamp(int currentTimestamp);
    void hideZoombar(bool hide);
    void stop();

private slots:
    void onPlayPauseBackwardClicked();
    void onPlayPauseForwardClicked();
    void onStopClicked();
    void onTimeForwardClicked();
    void onTimeBackwardClicked();
    void onSliderMoved(int value);
    void onSliderReleased();
    void onSliderValueChanged(int value);
    void onZoomSliderMoved(int);
    void onMinTimestampEditingFinished();
    void onMaxTimestampEditingFinished();
    void onCurrentTimestampEditingFinished();
    void onResetRangeBtnClicked();
    void onSpeedSliderValueChanged();
    void onSliderPressed();
    void onRepeatCheckBoxClicked(bool checked);

private:
    Ui::Trackbar *ui;
    void setSliderRanges(int minimum, int maximum);
    void setTsByTsToolRanges(int minimum, int maximum);
    void setRanges(int minimum, int maximum);
    void updateCurrentTimestamp(int currentTimestamp);
    void updateCurrentTimeLabel();
    void updateBackwardAndForwardBtns();
    void setTsByTsToolEnabled(bool enabled);
    void setTrackbarReadyToPlay(bool ready);
    void calcSliderRanges();
    void updatePlayBtns();

protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // TRACKBAR_H
