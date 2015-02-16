#-------------------------------------------------
#
# Project created by QtCreator 2013-11-01T10:13:02
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = xml_stream_writer
CONFIG   += console
CONFIG   -= app_bundle
#CONFIG   += debug
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

INCLUDEPATH += ../MultiTrackVideoPlayer/VideoData \
               ../MultiTrackVideoPlayer


SOURCES += xml_stream_writer.cpp \
           ../MultiTrackVideoPlayer/VideoData/videogop.cpp \
           ../MultiTrackVideoPlayer/VideoData/videopicture.cpp \
           ../MultiTrackVideoPlayer/VideoData/videotrackcollection.cpp \
           ../MultiTrackVideoPlayer/VideoData/videotrack.cpp \
           ../MultiTrackVideoPlayer/VideoData/xmlsupport.cpp
