#-------------------------------------------------
#
# Project created by QtCreator 2013-10-21T13:12:19
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MultiTrackVideoPlayer
TEMPLATE = app

SOURCES += main.cpp \
    Decoder/H264Decoder.cpp \
    Decoder/JpegDecoder.cpp \
    Exception.cpp \
    VideoData/videoimage.cpp \
    VideoData/videogop.cpp \
    VideoData/videopicture.cpp \
    VideoData/videotrack.cpp \
    VideoData/videotrackcollection.cpp \
    VideoData/xmlsupport.cpp \
    Backend/DecoderThread.cpp \
    gui/VideoWindow.cpp \
    Backend/PriorityQueue.cpp \
    gui/trackbar.cpp \
    Backend/VideoImageConsumer.cpp \
    Backend/SharedThreadData.cpp \
    PlaybackController.cpp \
    gui/mainwindow.cpp \
    Backend/State.cpp \
    Parser/rbsp.cpp \
    Parser/offsettype.cpp \
    Parser/h264parser.cpp \
    Parser/bitstream.cpp \
    Parser/parser.cpp \
    Backend/ThreadController.cpp \
    Backend/Timing.cpp \
    Decoder/DecoderFactory.cpp \
    Backend/StateHistory.cpp

HEADERS  += \
    Decoder/H264Decoder.h \
    Decoder/JpegDecoder.h \
    Decoder/IDecoder.h \
    VideoData/videoimage.h \
    VideoData/helpers_template.h \
    VideoData/ivideogop.h \
    VideoData/ivideopicture.h \
    VideoData/ivideotrack.h \
    VideoData/ivideotrackcollection.h \
    VideoData/videogop.h \
    VideoData/videopicture.h \
    VideoData/videotrack.h \
    VideoData/videotrackcollection.h \
    VideoData/xmlsupport.h \
    VideoData/ixmlsupport.h \
    Backend/DecoderThread.h \
    gui/VideoWindow.h \
    Exception.h \
    Backend/PriorityQueue.h \
    gui/trackbar.h \
    Backend/VideoImageConsumer.h \
    Backend/SharedThreadData.h \
    PlaybackController.h \
    gui/mainwindow.h \
    Backend/State.h \
    Parser/rbsp.h \
    Parser/offsettype.h \
    Parser/h264parser.h \
    Parser/bitstream.h \
    Parser/parser.h \
    Backend/ThreadController.h \
    Backend/Timing.h \
    Decoder/DecoderFactory.h \
    Enums.h \
    Backend/StateHistory.h

INCLUDEPATH += .

mac {
    INCLUDEPATH += /opt/local/include
    LIBS += -L/opt/local/lib
    CONFIG += x86_64


    QMAKE_INFO_PLIST = Info.plist

    # Icon is mandatory for submission
    #ICON = AppIcon.icns
}

linux {
    INCLUDEPATH += /usr/include
    LIBS += -L/usr/lib
}

win32 {
    INCLUDEPATH += U:/include
    LIBS += -LU:/lib
}

LIBS += -lavcodec                         \
        -lavdevice                        \
        -lavformat                        \
        -lavfilter                        \
        -lswresample                      \
        -lswscale                         \
        -lavutil                          \
        -lpthread

FORMS += gui/trackbar.ui \
    gui/mainwindow.ui

OTHER_FILES += \
    Info.plist \
    greyblue.qss

RESOURCES += \
    default.qrc
