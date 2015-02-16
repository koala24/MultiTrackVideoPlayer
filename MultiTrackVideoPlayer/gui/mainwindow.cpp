#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qRegisterMetaType<VideoImagePtr>("VideoImagePtr");
    ui->setupUi(this);
    setWindowTitle("MultiTrackVideoPlayer");
    _playbackController = new PlaybackController();
    _trackCollection = new VideoTrackCollection();
    connect(_playbackController, &PlaybackController::timestampChanged, ui->_trackbar, &Trackbar::jumpToTimestamp);
    connect(_playbackController, &PlaybackController::stopped, this, &MainWindow::onStopped);
    connect(_playbackController, &PlaybackController::buffering, this, &MainWindow::onBuffering);
    connect(_playbackController, &PlaybackController::playing, this, &MainWindow::onPlaying);
    connect(ui->_trackbar, &Trackbar::speedChanged, _playbackController, &PlaybackController::changePlaybackSpeed);
    connect(ui->_trackbar, &Trackbar::timestampChanged, _playbackController, &PlaybackController::jumpToTimestamp);
    connect(ui->_trackbar, &Trackbar::repeatClicked, _playbackController, &PlaybackController::enableRepeat);

    _lastWindowPos = QPoint(0, 0);
    _lastPath = QDir::homePath();
    _recentFiles = new QStringList();
    _timeLimitStatusBar = 5000;
    loadSettings();
    useStyleSheet();

    _viewGroup = new QActionGroup(this);
    _viewGroup->addAction(ui->actionShow_side_by_side);
    _viewGroup->addAction(ui->actionAnchored);
    _viewGroup->setExclusive(true);

    QActionGroup *languageGroup = new QActionGroup(this);
    languageGroup->addAction(ui->actionStandard);
    languageGroup->addAction(ui->actionGray_Blue);
    languageGroup->setExclusive(true);

    _signalMapper = new QSignalMapper(this);
    _maxRecentFiles = 7;
    fillRecentFilesMenu();
    connect(_signalMapper, SIGNAL(mapped(int)), this, SLOT(openTrackFromRecentFilesMenu(int)));

    ui->_videoWindowGroupBox->setVisible(false);
    ui->statusbar->showMessage(tr("MTVP!"), _timeLimitStatusBar);
}

MainWindow::~MainWindow()
{
    delete _playbackController;
    delete _trackCollection;
    delete _recentFiles;
    delete ui;
}

void MainWindow::onOpenClicked()
{
    ui->statusbar->showMessage(tr("Select Tracks ..."));
    QStringList files = QFileDialog::getOpenFileNames(0,
                                                      tr("Open Video-Files"),
                                                      _lastPath,
                                                      tr("XML (*.xml);;H264 (*.264);;JPEG (*.jpg)"));
    if (!files.isEmpty())
    {
        if (files.at(0).endsWith(".jpg") || files.at(0).endsWith(".264")) // h264 geht / jpeg noch nicht getestet~
        {
            QFileInfo fileInfo;
            QStringList xmlFiles;
            foreach (QString file, files)
            {
                Parser::createXMLFile(file, 24, QSize(640, 480)); // fps!
                fileInfo.setFile(file);
                if (files.at(0).endsWith(".264"))
                {
                    xmlFiles << fileInfo.absolutePath() + "/" + fileInfo.baseName() + ".xml";
                }
                else
                {
                    QDir dir(fileInfo.absolutePath());
                    QString baseName = fileInfo.baseName();
                    baseName.resize(baseName.size() - 4);
                    xmlFiles << dir.absolutePath() + "/" + baseName + ".xml";
                }
            }
            open(xmlFiles);
        }
        else
        {
            open(files);
        }
    }
}

void MainWindow::onStopClicked()
{
    _playbackController->enableRepeat( false );
    _playbackController->stop();
    _viewGroup->setEnabled(true);
    ui->statusbar->showMessage(tr("Stopped."));
}

void MainWindow::onStopped()
{
    disconnect(_playbackController, &PlaybackController::timestampChanged, ui->_trackbar, &Trackbar::jumpToTimestamp);
    ui->_trackbar->stop();
    _viewGroup->setEnabled(true);
    ui->_trackbar->setBufferingMessageEnabled(false);
    ui->statusbar->showMessage(tr(""));
}

void MainWindow::onPauseClicked()
{
    _playbackController->pause();
    _viewGroup->setEnabled(true);
    ui->_trackbar->setBufferingMessageEnabled(false);
    ui->statusbar->showMessage(tr("Paused."));
}

void MainWindow::onPlayForwardClicked()
{
    _playbackController->enableRepeat( ui->_trackbar->isRepeatChecked() );
    if (_playbackController->getCurrentPlayDirection() == PlayDirection::Backward)
    {
        _playbackController->changePlayDirection();
    }
    _playbackController->play();
    _viewGroup->setEnabled(false);
    ui->_trackbar->setBufferingMessageEnabled(false);
    ui->statusbar->showMessage(tr("Playing forward."));
}

void MainWindow::onPlayBackwardClicked()
{
    _playbackController->enableRepeat( ui->_trackbar->isRepeatChecked() );
    if (_playbackController->getCurrentPlayDirection() == PlayDirection::Forward)
    {
        _playbackController->changePlayDirection();
    }
    _playbackController->play();
    _viewGroup->setEnabled(false);
    ui->_trackbar->setBufferingMessageEnabled(false);
    ui->statusbar->showMessage(tr("Playing backward."));
}

void MainWindow::onBuffering()
{
    ui->statusbar->showMessage(tr("Buffering..."));
    ui->_trackbar->setBufferingMessageEnabled(true);
}

void MainWindow::onPlaying()
{
    connect(_playbackController, &PlaybackController::timestampChanged, ui->_trackbar, &Trackbar::jumpToTimestamp);
    _viewGroup->setEnabled(false);
    ui->_trackbar->setBufferingMessageEnabled(false);
    if (_playbackController->getCurrentPlayDirection() == PlayDirection::Forward)
    {
        ui->statusbar->showMessage(tr("Playing forward."));
    }
    if (_playbackController->getCurrentPlayDirection() == PlayDirection::Backward)
    {
        ui->statusbar->showMessage(tr("Playing backward."));
    }
}

void MainWindow::onSpeedChanged(int speed)
{
    _playbackController->changePlaybackSpeed(speed);
}

void MainWindow::onExitClicked()
{
    ui->_trackbar->setHalt(true);
    close();
}

void MainWindow::onAboutClicked()
{
    ui->statusbar->showMessage(tr("About MTVP..."));
    QMessageBox::about(this, tr("About MultiTrackVideoPlayer"), tr("<b>Multi-Track Videoplayer</b><br>" \
                                                                   "Projektstudium WS 2013/2014<br><br>" \
                                                                   "<b>Betreuer:</b><br>" \
                                                                   "Prof. Dr. Frank Bauernöppel<br><br>" \
                                                                   "<b>Projektgruppe:</b><br>"
                                                                   "Robert Jacob (s0519543)<br>" \
                                                                   "Sabrina Milewsky (s0535425)<br>" \
                                                                   "Alexej Osmolovsky (s0524752)<br>" \
                                                                   "Eugen Steinepress (s0527396)<br>" \
                                                                   "Jan Pokiser (s0533721)<br>" \
                                                                   "Adrian Helbig (s0531966)"));
    ui->statusbar->showMessage(tr("MTVP!"), _timeLimitStatusBar);
}

void MainWindow::onActionAnchorToggled(bool toggled)
{
    if (toggled)
    {
        if (_videoWindowMap.size() > 0)
        {
            ui->_videoWindowGroupBox->setVisible(true);
        }
        addVideoWindowsToGridLayout();
        if (_trackCollection->getVideoTracks().size() > 0 && !isMaximized())
        {
            showMaximized();
            QCoreApplication::processEvents(QEventLoop::AllEvents, 200); // kurz warten bis das Fenster tatsächlich maximiert ist, sonst hängt es (isMaximized geht nicht)
            int maxTs = ui->_trackbar->getMaxTimestamp(); // damit sich die fenster ausrichten
            _playbackController->jumpToTimestamp(maxTs);
            _playbackController->jumpToTimestamp(0);
        }
        ui->statusbar->showMessage(tr("View changed."), _timeLimitStatusBar);
    }
}

void MainWindow::onActionSideBySideToggled(bool toggled)
{
    if (toggled)
    {
        ui->_videoWindowGroupBox->setVisible(false);
        removeVideoWindowsFromGridLayout();
        if (_trackCollection->getVideoTracks().size() > 0 && isMaximized())
        {
            this->setWindowFlags(Qt::Window);
            showNormal();
            resize(0, 0);
            move(30, 30);
        }
        ui->statusbar->showMessage(tr("View changed."), _timeLimitStatusBar);
    }
}

void MainWindow::onActionZoombarToggled(bool toggled)
{
    ui->_trackbar->hideZoombar(toggled);
}

void MainWindow::useStyleSheet()
{
    QFile *stylesheet = NULL;
    if (!ui->actionStandard->isChecked())
    {
        if (ui->actionGray_Blue->isChecked())
        {
            stylesheet = new QFile(":/stylesheets/grayBlue");
        }

        if (stylesheet->open(QIODevice::ReadOnly))
        {
            setStyleSheet(stylesheet->readAll());
            stylesheet->close();
            ui->statusbar->showMessage(tr("Look and Feel changed."), _timeLimitStatusBar);
        }
        else
        {
            qDebug() << "stylesheet could not be loaded";
        }
        if (stylesheet != NULL)
        {
            delete stylesheet;
        }
    }
    else
    {
        setStyleSheet("");
        ui->statusbar->showMessage(tr("Look and Feel changed."), _timeLimitStatusBar);
    }
}

void MainWindow::openTrackFromRecentFilesMenu(int index)
{
    open(_recentFiles->at(index));
}

void MainWindow::onClearRecentFiles()
{
    _recentFiles->clear();
    ui->menuRecent_Files->clear();
    ui->statusbar->showMessage(tr("Cleared recent files list."), _timeLimitStatusBar);
}

void MainWindow::arrangeWindow( VideoTrack *track ) {
    QDesktopWidget deskWidget;
    QSize trackSize = track->getVideoSize();
    VideoWindow *window = track->getAssignedWindow();
    float width = trackSize.width() - (deskWidget.width() * 0.02);
    float height = trackSize.height() - (deskWidget.height() * 0.02);
    for (int i = 100; width * 2 >= deskWidget.width() && i > 0; i -= 5)
    {
        width = (trackSize.width() - (deskWidget.width() * 0.02)) * i / 100;
        height = (trackSize.height() - (deskWidget.height() * 0.02)) * i / 100;
    }
    window->move(_lastWindowPos);
    window->resize(width, height);
    window->setWindowTitle(track->getTrackName());
    int x = _lastWindowPos.x() + width;
    int y = _lastWindowPos.y();
    if ( x + width >= deskWidget.width() ) {
        x = 0;
        y = _lastWindowPos.y() + height;
    }

    if ( y + height >= deskWidget.height() )
        x = y = 0;

    _lastWindowPos.setX( x );
    _lastWindowPos.setY( y );
}

void MainWindow::arrangeWindow()
{
    _lastWindowPos = QPoint(0, 0);
    foreach (IVideoTrack *itrack, _trackCollection->getVideoTracks())
    {
        VideoTrack *track = dynamic_cast<VideoTrack*>(itrack);
        arrangeWindow(track);
    }
}

void MainWindow::open(QString file)
{
    open(QStringList(file));
}

void MainWindow::open(QStringList files)
{
    VideoTrack *track = NULL;
    VideoWindow *window = NULL;
    QFileInfo fileInfo;
    if (!files.isEmpty())
    {
        fileInfo = QFileInfo(files.at(0));
        _lastPath = fileInfo.absolutePath();
        foreach (QString file, files)
        {
            fileInfo = QFileInfo(file);
            track = new VideoTrack();
            if (fileInfo.exists() && track->readXMLFile(file))
            {
                track->setFileInfo(fileInfo);
                if ( _playbackController->addTrack(track) ) {
                    _trackCollection->addVideoTrack(track);
                    appendToRecentFileMenu(fileInfo);
                    int videoWindowCount = _videoWindowMap.size();
                    window = new VideoWindow(videoWindowCount, 0);
                    _videoWindowMap.insert(videoWindowCount, window);
                    track->setAssignedWindow(window);
                    window->show();
                }
            }
            else
            {
                qDebug() << "xml file could not be loaded";
            }
        }
        fillRecentFilesMenu();
        int maxTs = _trackCollection->findTheHighestTimestamp();
        _playbackController->setMaxTimestamp(maxTs);
        ui->_trackbar->setMaxTimestamp(maxTs);
        if (ui->actionAnchored->isChecked())
        {
            onActionAnchorToggled(true);
        }
        else
        {
            arrangeWindow();
            showNormal();
            raise();
            activateWindow();
        }
        ui->statusbar->showMessage(tr("Tracks loaded."), _timeLimitStatusBar);
    }
    else
    {
        qDebug() << "files are empty";
    }
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    if (isMaximized())
    {
        settings.setValue("windowState", 1);
    }
    else
    {
        settings.setValue("windowState", 0);
    }
    settings.setValue("size", this->size());
    settings.setValue("position", this->pos());
    settings.setValue("lastPath", _lastPath);

    if (ui->actionStandard->isChecked())
    {
        settings.setValue("style", 0);
    }
    if (ui->actionGray_Blue->isChecked())
    {
        settings.setValue("style", 1);
    }

    settings.setValue("zoombar", ui->actionZoombar->isChecked());

    settings.beginWriteArray("recentFiles");
    int count = 0;
    for (int i = _recentFiles->size() - 1; i >= 0 && _recentFiles->size() - i <= _maxRecentFiles; i--)
    {
        settings.setArrayIndex(count);
        settings.setValue("path", _recentFiles->at(i));
        count++;
    }
    settings.endArray();

    settings.endGroup();
}

void MainWindow::loadSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    int windowState = settings.value("windowState", 0).toInt();
    if (windowState == 0)
    {
        resize(settings.value("size", QSize(0, 0)).toSize());
        move(settings.value("position", QPoint(30, 30)).toPoint());
    }
    else
    {
        showNormal();
        resize(0, 0);
        move(30, 30);
    }
    _lastPath = settings.value("lastPath", QString(_lastPath)).toString();

    int style = settings.value("style", 1).toInt();
    if (style == 0)
    {
        ui->actionStandard->setChecked(true);
    }
    if (style == 1)
    {
        ui->actionGray_Blue->setChecked(true);
    }
    useStyleSheet();

    bool zoombar = settings.value("zoombar", true).toBool();
    ui->_trackbar->hideZoombar(zoombar);
    ui->actionZoombar->setChecked(zoombar);

    int size = settings.beginReadArray("recentFiles");
    for (int i = size -1; i >= 0; i--)
    {
        settings.setArrayIndex(i);
        _recentFiles->append(settings.value("path").toString());
    }

    settings.endArray();

    settings.endGroup();
}

void MainWindow::addVideoWindowsToGridLayout()
{
    if (!_videoWindowMap.isEmpty())
    {
        if (_videoWindowMap.size() <= 4)
        {
            insertVideoWindowGridLayout(2);
        }
        else
        {
            insertVideoWindowGridLayout(3);
        }
    }
}

void MainWindow::insertVideoWindowGridLayout(int columnCount)
{
    VideoWindow *videoWindow = NULL;
    int row = 0;
    int column = 1;
    for (QMap<int, VideoWindow*>::const_iterator it = _videoWindowMap.constBegin(); it != _videoWindowMap.constEnd(); ++it)
    {
        videoWindow = it.value();
        if (videoWindow != NULL)
        {
            videoWindow->setScalingEnabled(true);
            ui->_videoWindowGridLayout->addWidget(videoWindow, row, column);
            if (column % columnCount == 0)
            {
                column = 1;
                row++;
            }
            else
            {
                column++;
            }
        }
    }
}

void MainWindow::removeVideoWindowsFromGridLayout()
{
    VideoWindow *videoWindow = NULL;
    for (QMap<int, VideoWindow*>::const_iterator it = _videoWindowMap.constBegin(); it != _videoWindowMap.constEnd(); ++it)
    {
        videoWindow = it.value();
        if (videoWindow != NULL)
        {
            videoWindow->setParent(0);
            videoWindow->show();
        }
    }
    arrangeWindow();
}

void MainWindow::fillRecentFilesMenu()
{
    ui->menuRecent_Files->clear();
    for (int i = _recentFiles->size() - 1; i >= 0 && _recentFiles->size() - i <= _maxRecentFiles; i--)
    {
        QAction *action = new QAction("../" + QFileInfo(_recentFiles->at(i)).fileName(), this);
        connect(action, SIGNAL(triggered()), _signalMapper, SLOT(map()));
        _signalMapper->setMapping(action, i);
        ui->menuRecent_Files->addAction(action);
    }
    if (_recentFiles->size() > 0)
    {
        QAction *actionClearRecentFiles = new QAction("clear", this);
        ui->menuRecent_Files->addSeparator();
        ui->menuRecent_Files->addAction(actionClearRecentFiles);
        connect(actionClearRecentFiles, &QAction::triggered, this, &MainWindow::onClearRecentFiles);
    }
}

void MainWindow::appendToRecentFileMenu(const QFileInfo fileInfo)
{
    bool exist = false;
    foreach (QAction* action, ui->menuRecent_Files->actions())
    {
        if (action->text() == "../" + fileInfo.fileName())
        {
            exist = true;
            break;
        }
    }
    if (!exist)
    {
        _recentFiles->append(fileInfo.absoluteFilePath());
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->_trackbar->setHalt( true );
    _playbackController->stop();

    VideoWindow *videoWindow = NULL;
    for (QMap<int, VideoWindow*>::const_iterator it = _videoWindowMap.constBegin(); it != _videoWindowMap.constEnd(); ++it)
    {
        videoWindow = it.value();
        if (videoWindow != NULL)
        {
            videoWindow->close();
        }
    }

    saveSettings();
    event->accept();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *styleMenu = new QMenu("Appearance", this);
    styleMenu->addAction(ui->actionStandard);
    styleMenu->addAction(ui->actionGray_Blue);

    QMenu menu(this);
    menu.addAction(ui->actionOpen_Files);
    menu.addSeparator();
    menu.addMenu(styleMenu);
    menu.addSeparator();
    menu.addAction(ui->actionShow_side_by_side);
    menu.addAction(ui->actionAnchored);
    menu.addSeparator();
    menu.addAction(ui->actionZoombar);
    menu.exec(event->globalPos());
}
