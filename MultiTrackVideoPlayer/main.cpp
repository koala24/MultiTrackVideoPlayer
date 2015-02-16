#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QThread>
#include <QElapsedTimer>
#include <QtDebug>

#include <stdio.h>

#include "gui/mainwindow.h"

static QFile *file;
static QTextStream *stream;
static QMutex *streamMutex;
static QElapsedTimer *logTimer;

bool loggingExtreme;

void createLogFile() {
    file = new QFile( QDir::homePath() + QDir::separator() + "mtvp.log" );
    if ( !file->open( QIODevice::WriteOnly ) ) {
        delete file;
        file = 0;
        return;
    } else {
        stream = new QTextStream( file );
    }
}

void closeLogFile() {
    if ( file != 0 ) {
        file->close();
        delete stream;
        delete file;
    }
}

QString translateQThreadPointer( QThread *ptr ) {
    QString ret;
    if ( dynamic_cast<DecoderThread *>( ptr ) != NULL )
        ret = QString( "DecoderThread #%1" ).arg( dynamic_cast<DecoderThread *>( ptr )->getThreadId() );
    else if ( dynamic_cast<VideoImageConsumer *>( ptr ) != NULL )
        ret = "ConsumerThread";
    else
        ret = "MainThread";

    return ret;
}

void logMessageHandler( QtMsgType /* type */, const QMessageLogContext &context, const QString &msg ) {
    streamMutex->lock();
    QString message;
    QString moreInfos;
    if ( loggingExtreme ) {
        moreInfos = QString( " (from: %1:%2)" ).
                            arg( context.function ).
                            arg( context.line );
    }

    message = message.sprintf( "%08lld @%-17s: %s%s\n",
                                logTimer->elapsed(),
                                qPrintable( translateQThreadPointer( QThread::currentThread() ) ),
                                qPrintable( msg ),
                                qPrintable( moreInfos ) );
    if ( file != 0 )
        *stream << message;
    streamMutex->unlock();

    fprintf( stderr, "%s", qPrintable( message ) );
    fflush( stderr );
}

int main( int argc, char **argv ) {
    QApplication application( argc, argv );

    QCoreApplication::setOrganizationName( "HTW Berlin" );
    QCoreApplication::setOrganizationDomain( "htw-berlin.de" );
    QCoreApplication::setApplicationName( "MultiTrackVideoPlayer" );

    streamMutex = new QMutex();
    logTimer = new QElapsedTimer();
    logTimer->start();

    loggingExtreme = qApp->arguments().contains( "--logging-extreme" );
    bool logToFile = !qApp->arguments().contains( "--no-filelog" );
    if ( logToFile )
        createLogFile();

    qInstallMessageHandler( logMessageHandler );
    qDebug() << "ello $Id$";

#if defined(Q_OS_MAC) && QT_VERSION < 0x050200
    QFont::insertSubstitution( ".Lucida Grande UI", "Lucida Grande" );
#endif

    MainWindow window;
    window.show();

    int retVal = application.exec();

    qInstallMessageHandler( NULL );

    if ( logToFile )
        closeLogFile();

    delete logTimer;
    delete streamMutex;

    return retVal;
}
