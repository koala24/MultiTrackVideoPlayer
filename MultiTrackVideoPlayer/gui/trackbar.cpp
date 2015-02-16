#include "trackbar.h"
#include "ui_trackbar.h"

Trackbar::Trackbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Trackbar)
{
    ui->setupUi(this);
    init();
}

Trackbar::Trackbar(int maxTimestamp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Trackbar)
{
    ui->setupUi(this);
    init(maxTimestamp);
}

Trackbar::~Trackbar()
{
    delete _loadingGif;
    delete ui;
}

void Trackbar::setMaxTimestamp(int maxTimestamp)
{
    _maxTimestamp = maxTimestamp;
    _maxTime = QTime(0, 0, 0, 0);
    _maxTime = _maxTime.addSecs(_maxTimestamp / 1000);
    ui->_maxTimeLabel->setText(_maxTime.toString());
    setTrackbarReadyToPlay(true); // falls maxTime > 0
    setRanges(0, _maxTimestamp);
    //ui->_zoomSlider->setMaximum(maxTimestamp);
    ui->_zoomSlider->setValue(ui->_zoomSlider->maximum());
}

int Trackbar::getMaxTimestamp()
{
    return _maxTimestamp;
}

void Trackbar::jumpToTimestamp(int currentTimestamp)
{
    if (currentTimestamp <= _maxTimestamp)
    {
        updateTrackbarCurrentTimestamp(currentTimestamp);
    }
}

void Trackbar::hideZoombar(bool hide)
{
    ui->_zoomGroupBox->setVisible(hide);
}

void Trackbar::stop()
{
    updateCurrentTimestamp(0);
    ui->_zoomSlider->setValue(ui->_zoomSlider->maximum());
    setRanges(0, _maxTimestamp);
    if (!_halt && ui->_repeatCheckBox->isChecked())
    {
        if (ui->_playPauseForwardBtn->isChecked())
        {
            emit playForwardClicked();
        }
        else if (ui->_playPauseBackwardBtn->isChecked())
        {
            int timestamp = _maxTimestamp - 1;
            ui->_slider->setValue(timestamp);
            emit timestampChanged(timestamp);
            ui->_playPauseBackwardBtn->setChecked(true);
            onPlayPauseBackwardClicked();
        }
    }
    else
    {
        setTrackbarReadyToPlay(true);
    }
}

void Trackbar::onPlayPauseForwardClicked()
{
    if (ui->_playPauseForwardBtn->isChecked())
    {
        ui->_playPauseBackwardBtn->setEnabled(false);
        ui->_playPauseBackwardBtn->setChecked(false);
        ui->_stopBtn->setEnabled(true);
        setTsByTsToolEnabled(false);
        emit playForwardClicked();
    }
    else
    {
        ui->_playPauseBackwardBtn->setEnabled(true);
        setTsByTsToolEnabled(true);
        emit pauseClicked();
    }
}

void Trackbar::onPlayPauseBackwardClicked()
{
    if (ui->_playPauseBackwardBtn->isChecked())
    {
        ui->_playPauseForwardBtn->setEnabled(false);
        ui->_playPauseForwardBtn->setChecked(false);
        ui->_stopBtn->setEnabled(true);
        setTsByTsToolEnabled(false);
        emit playBackwardClicked();
    }
    else
    {
        ui->_playPauseForwardBtn->setEnabled(true);
        setTsByTsToolEnabled(true);
        emit pauseClicked();
    }
}

void Trackbar::onStopClicked()
{
    ui->_playPauseBackwardBtn->setChecked(false);
    ui->_playPauseForwardBtn->setChecked(false);
    ui->_playPauseBackwardBtn->setEnabled(true);
    ui->_playPauseForwardBtn->setEnabled(true);

    ui->_stopBtn->setEnabled(false);
    updateTrackbarCurrentTimestamp(0);
    setTrackbarReadyToPlay(true);

    emit stopClicked();
}

void Trackbar::onTimeForwardClicked()
{
    if (_currentTimestamp < _maxTimestamp)
    {
        if(_currentTimestamp + _resolution > _maxTimestamp)
        {
            _currentTimestamp = _maxTimestamp;
        }
        else
        {
            _currentTimestamp += _resolution;
        }
        ui->_currentTimestampLineEdit->setText(QString::number(_currentTimestamp));
        ui->_slider->setValue(_currentTimestamp);
        emit timestampChanged(_currentTimestamp);
    }
    updateBackwardAndForwardBtns();
}

void Trackbar::onTimeBackwardClicked()
{
    if (_currentTimestamp > 0)
    {
        if(_currentTimestamp - _resolution < 0)
        {
            _currentTimestamp = 0;
        }
        else
        {
            _currentTimestamp -= _resolution;
        }
        ui->_currentTimestampLineEdit->setText(QString::number(_currentTimestamp));
        ui->_slider->setValue(_currentTimestamp);
        emit timestampChanged(_currentTimestamp);
    }
    updateBackwardAndForwardBtns();
}

void Trackbar::onSliderMoved(int value)
{
    if (!_hasPlayed)
    {
        if (ui->_playPauseBackwardBtn->isChecked())
        {
            _lastPlayDirection = PlayDirection::Backward;
            _hasPlayed = true;
            emit pauseClicked();
        }
        if (ui->_playPauseForwardBtn->isChecked())
        {
            _lastPlayDirection = PlayDirection::Forward;
            _hasPlayed = true;
            emit pauseClicked();
        }
    }
    QToolTip::showText(QCursor::pos(), _currentTime.toString());
    emit sliderMoved(value);
}

void Trackbar::onSliderReleased()
{
    if (_hasPlayed)
    {
        if (_lastPlayDirection == PlayDirection::Backward)
        {
            onPlayPauseBackwardClicked();
        }
        if (_lastPlayDirection == PlayDirection::Forward)
        {
            onPlayPauseForwardClicked();
        }
        _hasPlayed = false;
    }
}

void Trackbar::onSliderValueChanged(int value)
{
    updateTrackbarCurrentTimestamp(value);
}

void Trackbar::onZoomSliderMoved(int /* value */)
{
    calcSliderRanges();
}

void Trackbar::onMinTimestampEditingFinished()
{
    int min = ui->_minTimestampLineEdit->text().toInt();
    int max = ui->_maxTimestampLineEdit->text().toInt();
    if (min >= 0 && min < max)
    {
        setRanges(min, max);
    }
    else
    {
        ui->_minTimestampLineEdit->setText("0");
    }
}

void Trackbar::onMaxTimestampEditingFinished()
{
    int max = ui->_maxTimestampLineEdit->text().toInt();
    int min = ui->_minTimestampLineEdit->text().toInt();
    if (max <= _maxTimestamp && max >= min)
    {
        setRanges(min, max);
    }
    else
    {
        ui->_maxTimestampLineEdit->setText(QString::number(_maxTimestamp));
    }
}

void Trackbar::onCurrentTimestampEditingFinished()
{
    int currentTimestamp = ui->_currentTimestampLineEdit->text().toInt();
    if (currentTimestamp >= 0 && currentTimestamp <= _maxTimestamp)
    {
        updateTrackbarCurrentTimestamp(currentTimestamp);
        emit timestampChanged(currentTimestamp);
    }
    else
    {
        ui->_currentTimestampLineEdit->setText(QString::number(_currentTimestamp));
    }
}

void Trackbar::onResetRangeBtnClicked()
{
    ui->_zoomSlider->setValue(ui->_zoomSlider->maximum());
    setRanges(0, _maxTimestamp);
}

/*
    speed from 1 to 16 divided by 4
*/
void Trackbar::onSpeedSliderValueChanged(){
    qreal map[] = {8.0, 4.0, 2.0, 1.0, 0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625};
    //qreal speed = (qreal)ui->_speedSlider->value() / 4;
    qreal speed = 1 / map[ui->_speedSlider->value() - 1];
    ui->_currentSpeedLineEdit->setText(QString::number(speed, 'g', 3) + 'x');

    //qDebug() << ui->_speedSlider->value() << speed << map[ui->_speedSlider->value() - 1];
    emit speedChanged(map[ui->_speedSlider->value() - 1]);
}

void Trackbar::onSliderPressed()
{
    _sliderPressed = true;
}

void Trackbar::init(int maxTimestamp)
{
    _maxTimestamp = maxTimestamp;
    _currentTimestamp = 0;

    _currentTime = QTime(0, 0, 0, 0);
    _maxTime = QTime(0, 0, 0, 0);
    _maxTime = _maxTime.addSecs(_maxTimestamp / 1000);
    _resolution = 40; // should be the minimum of (fps / 1000)
    _sliderPressed = false;
    _hasPlayed = false;
    _halt = false;

    _loadingGif = new QMovie(":/icons/buffering");
    ui->_bufferingGifLabel->setMovie(_loadingGif);
    ui->_bufferingLabel->setText("Buffering");
    ui->_bufferingSpaceLabel->setMinimumWidth(ui->_bufferingLabel->width() - 12);
    setBufferingMessageEnabled(false);

    updateCurrentTimeLabel();
    ui->_maxTimeLabel->setText(_maxTime.toString());
    ui->_slider->setTickInterval(_resolution);
    ui->_slider->setRange(0, _resolution);
    ui->_slider->installEventFilter(this);
    setSliderRanges(0, _maxTimestamp);

    ui->_minTimestampLineEdit->setText(QString::number(0));
    ui->_minTimestampLineEdit->setToolTip(tr("Minimum Timestamp"));
    ui->_currentTimestampLineEdit->setText(QString::number(0));
    ui->_currentTimestampLineEdit->setToolTip(tr("Current Timestamp"));
    ui->_maxTimestampLineEdit->setText(QString::number(_maxTimestamp));
    ui->_maxTimestampLineEdit->setToolTip(tr("Maximum Timestamp"));

    QIcon icon;
    icon.addPixmap(QPixmap(":/icons/play"), QIcon::Normal, QIcon::Off);
    icon.addPixmap(QPixmap(":/icons/pause"), QIcon::Normal, QIcon::On);
    ui->_playPauseForwardBtn->setIcon(icon);
    ui->_playPauseForwardBtn->setCheckable(true);
    ui->_playPauseForwardBtn->setChecked(false);

    icon = QIcon();
    icon.addPixmap(QPixmap(":/icons/backwardPlay"), QIcon::Normal, QIcon::Off);
    icon.addPixmap(QPixmap(":/icons/pause"), QIcon::Normal, QIcon::On);
    ui->_playPauseBackwardBtn->setIcon(icon);
    ui->_playPauseBackwardBtn->setCheckable(true);
    ui->_playPauseBackwardBtn->setChecked(false);

    ui->_speedSlider->setTickInterval(1);
    ui->_speedSlider->setValue(4);
    ui->_speedSlider->setEnabled(false);
    ui->_currentSpeedLineEdit->setText("1.0x");

    setTrackbarReadyToPlay(true);

    connect(ui->_repeatCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(onRepeatCheckBoxClicked(bool)));
}

void Trackbar::setBufferingMessageEnabled(bool buffering)
{
    if (buffering)
    {
        _loadingGif->start();
    }
    else
    {
        _loadingGif->stop();
    }
    ui->_bufferingGifLabel->setVisible(buffering);
    ui->_bufferingLabel->setVisible(buffering);
    ui->_bufferingSpaceLabel->setVisible(buffering);
}

void Trackbar::setSliderRanges(int minimum, int maximum)
{
    ui->_slider->setMinimum(minimum);
    ui->_slider->setMaximum(maximum);
    ui->_slider->update();
}

void Trackbar::setTsByTsToolRanges(int minimum, int maximum)
{
    ui->_minTimestampLineEdit->setText(QString::number(minimum));
    ui->_maxTimestampLineEdit->setText(QString::number(maximum));
}

void Trackbar::setRanges(int minimum, int maximum)
{
    setTsByTsToolRanges(minimum, maximum);
    setSliderRanges(minimum, maximum);
}

void Trackbar::updateTrackbarCurrentTimestamp(int currentTimestamp)
{
    if (currentTimestamp >= 0 && currentTimestamp < _maxTimestamp)
    {
        _currentTimestamp = currentTimestamp;
        ui->_slider->setValue(_currentTimestamp);
        updateCurrentTimestamp(_currentTimestamp);
        updateCurrentTimeLabel();
        updateBackwardAndForwardBtns();

        if((ui->_slider->value() == ui->_slider->maximum() || ui->_slider->value() == ui->_slider->minimum()) && !_sliderPressed)
            calcSliderRanges();

        ui->_currentTimestampLineEdit->setText(QString::number(_currentTimestamp));
    }
}

void Trackbar::updateCurrentTimestamp(int currentTimestamp)
{
    _currentTime = QTime(0, 0, 0, 0);
    _currentTime = _currentTime.addSecs(currentTimestamp / 1000);
}

void Trackbar::updateCurrentTimeLabel()
{
    ui->_currentTimeLabel->setText(_currentTime.toString());
}

void Trackbar::updateBackwardAndForwardBtns()
{
    if (_currentTimestamp > 0)
    {
        ui->_backwardBtn->setEnabled(true);
    }

    if (_currentTimestamp == _maxTimestamp || _currentTimestamp + _resolution > _maxTimestamp + _resolution)
    {
        ui->_forwardBtn->setEnabled(false);
    }
    else
    {
        ui->_forwardBtn->setEnabled(true);
    }

    if (_currentTimestamp == 0 || _currentTimestamp - _resolution < 0 - _resolution)
    {
        ui->_backwardBtn->setEnabled(false);
    }

    if (ui->_playPauseForwardBtn->isChecked() || ui->_playPauseBackwardBtn->isChecked())
    {
        ui->_forwardBtn->setEnabled(false);
        ui->_backwardBtn->setEnabled(false);
    }
    updatePlayBtns();
}

void Trackbar::updatePlayBtns()
{
    if(_currentTimestamp == _maxTimestamp || _currentTimestamp + _resolution > _maxTimestamp + _resolution)
    {
        ui->_playPauseForwardBtn->setEnabled(false);
        ui->_stopBtn->setEnabled(false);
    }
    else
    {
        if (!(ui->_playPauseBackwardBtn->isChecked()))
          ui->_playPauseForwardBtn->setEnabled(true);
        ui->_stopBtn->setEnabled(true);
    }

    if(_currentTimestamp == 0 || _currentTimestamp - _resolution < 0)
    {
        ui->_stopBtn->setEnabled(false);
        ui->_playPauseBackwardBtn->setEnabled(false);
    }
    else
    {
        if(!(ui->_playPauseForwardBtn->isChecked()))
          ui->_playPauseBackwardBtn->setEnabled(true);
        ui->_stopBtn->setEnabled(true);
    }
}


void Trackbar::setTsByTsToolEnabled(bool enabled)
{
    if (enabled && _currentTimestamp > 0)
    {
        ui->_backwardBtn->setEnabled(true);
    }
    else
    {
        ui->_backwardBtn->setEnabled(false);
    }

    ui->_currentTimestampLineEdit->setEnabled(enabled);

    if (enabled && _currentTimestamp < _maxTimestamp)
    {
        ui->_forwardBtn->setEnabled(true);
    }
    else
    {
        ui->_forwardBtn->setEnabled(false);
    }
}

void Trackbar::setTrackbarReadyToPlay(bool ready)
{
    ui->_playPauseBackwardBtn->setChecked(false);
    ui->_playPauseForwardBtn->setChecked(false);
    if (ready && _maxTimestamp > 0)
    {
        ui->_playPauseBackwardBtn->setEnabled(false);
        ui->_stopBtn->setEnabled(false);
        ui->_playPauseForwardBtn->setEnabled(true);
        ui->_currentTimeLabel->setEnabled(true);
        ui->_maxTimeLabel->setEnabled(true);
        ui->_slider->setEnabled(true);
        ui->_slider->setValue(0);
        ui->_zoomSlider->setEnabled(true);
        ui->_minTimestampLineEdit->setEnabled(true);
        setTsByTsToolEnabled(true);
        ui->_maxTimestampLineEdit->setEnabled(true);
        ui->_speedSlider->setEnabled(true);
        ui->_currentSpeedLineEdit->setEnabled(true);
        ui->_currentTimestampLineEdit->setText("0");
    }
    else
    {
        ui->_playPauseBackwardBtn->setEnabled(false);
        ui->_stopBtn->setEnabled(false);
        ui->_playPauseForwardBtn->setEnabled(false);
        ui->_currentTimeLabel->setEnabled(false);
        ui->_maxTimeLabel->setEnabled(false);
        ui->_slider->setEnabled(false);
        ui->_slider->setValue(0);
        ui->_zoomSlider->setEnabled(false);
        ui->_minTimestampLineEdit->setEnabled(false);
        setTsByTsToolEnabled(false);
        ui->_maxTimestampLineEdit->setEnabled(false);
        ui->_speedSlider->setEnabled(false);
        ui->_currentSpeedLineEdit->setEnabled(true);
        ui->_currentTimestampLineEdit->setText("0");
    }
}

void Trackbar::calcSliderRanges(){
    int pos = ui->_slider->value();
    int dialVal = ui->_zoomSlider->value();

    int min = pos - (qreal)(_maxTimestamp * dialVal / 100) / 2;
    int max = pos + (qreal)(_maxTimestamp * dialVal / 100) / 2;

    if (min < 0){
        max = max + min * (-1);
        min = 0;
    }

    if (max > _maxTimestamp) max = _maxTimestamp;

    setRanges(min, max);
}

bool Trackbar::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->_slider && event->type() == QEvent::MouseButtonRelease)
    {
        int sliderValue = 0;
        QMouseEvent *mouseEvent  = static_cast<QMouseEvent *>(event);
        if (_sliderPressed)
        {
            sliderValue = ui->_slider->value();
            onSliderValueChanged(sliderValue);
            _sliderPressed = false;
        }
        else
        {
            sliderValue = QStyle::sliderValueFromPosition(ui->_slider->minimum(), ui->_slider->maximum(), mouseEvent->x(), ui->_slider->width());
            onSliderValueChanged(sliderValue);
        }
        updateBackwardAndForwardBtns();
        updatePlayBtns();

        emit timestampChanged(sliderValue);
    }
    return false;
}

void Trackbar::setHalt(bool halt){
    _halt = halt;
}

bool Trackbar::isRepeatChecked()
{
    return ui->_repeatCheckBox->isChecked();
}

void Trackbar::onRepeatCheckBoxClicked(bool checked)
{
    emit repeatClicked( checked );
}
