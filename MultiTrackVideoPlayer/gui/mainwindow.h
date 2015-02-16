#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QDesktopWidget>
#include <QPoint>
#include <QFileInfo>
#include <QSettings>
#include <QLayoutItem>
#include <QGridLayout>
#include <QMap>
#include <QActionGroup>
#include <QSignalMapper>

#include "Backend/DecoderThread.h"
#include "PlaybackController.h"
#include "VideoData/videotrackcollection.h"
#include "Parser/parser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QMap<int, VideoWindow*> _videoWindowMap;
    DecoderThread *_decoderThread;
    PlaybackController *_playbackController;
    IVideoTrackCollection *_trackCollection;
    QPoint _lastWindowPos;
    QString _lastPath;
    QActionGroup *_viewGroup;
    QStringList *_recentFiles;
    int _maxRecentFiles;
    QSignalMapper *_signalMapper;
    int _timeLimitStatusBar;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onOpenClicked();
    void onStopClicked();
    void onStopped();
    void onPauseClicked();
    void onPlayForwardClicked();
    void onPlayBackwardClicked();
    void onBuffering();
    void onPlaying();
    void onSpeedChanged(int);
    void onExitClicked();
    void onAboutClicked();
    void onActionAnchorToggled(bool toggled);
    void onActionSideBySideToggled(bool toggled);
    void onActionZoombarToggled(bool toggled);
    void useStyleSheet();
    void openTrackFromRecentFilesMenu(int index);
    void onClearRecentFiles();

private:
    Ui::MainWindow *ui;
    void arrangeWindow( VideoTrack *track );
    void arrangeWindow();
    void open(QString file);
    void open(QStringList files);
    void saveSettings();
    void loadSettings();
    void addVideoWindowsToGridLayout();
    void insertVideoWindowGridLayout(int columnCount);
    void removeVideoWindowsFromGridLayout();
    void fillRecentFilesMenu();
    void appendToRecentFileMenu(const QFileInfo fileInfo);

protected:
     void closeEvent(QCloseEvent *event);
     void contextMenuEvent(QContextMenuEvent *event);
};

#endif // MAINWINDOW_H
