#-------------------------------------------------
#
# Project created by QtCreator 2013-11-09T04:50:52
#
#-------------------------------------------------

QT       += core widgets

TARGET = QMultiTrackVideoPlayer
TEMPLATE = app

INCLUDEPATH += ../MultiTrackVideoPlayer/

SOURCES += main.cpp\
        ../MultiTrackVideoPlayer/VideoData/videoimage.cpp \
        mainwindow.cpp \
           ../MultiTrackVideoPlayer/VideoData/videogop.cpp \
           ../MultiTrackVideoPlayer/VideoData/videopicture.cpp \
           ../MultiTrackVideoPlayer/VideoData/videotrackcollection.cpp \
           ../MultiTrackVideoPlayer/VideoData/videotrack.cpp \
           ../MultiTrackVideoPlayer/VideoData/xmlsupport.cpp


HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
