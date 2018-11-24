#include <QCoreApplication>
#include "logthread.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch (type)
    {
    case QtDebugMsg:
    case QtInfoMsg:
    case QtWarningMsg:
    case QtCriticalMsg:
        logSpace::logThread::getInterface()->addDateToQueue(logSpace::logItem(type,msg));
        break;

    case QtFatalMsg:
        QByteArray localMsg = msg.toLocal8Bit();
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef QT_NO_DEBUG
    qInstallMessageHandler(myMessageOutput);
#endif

    qDebug() << __FILE__ << __func__ << __LINE__;

#ifdef QT_NO_DEBUG
    logSpace::logThread *log = logSpace::logThread::getInterface();
    QObject::connect(log,SIGNAL(finished()),log,SLOT(deleteLater()));
    log->start();
#endif

    return a.exec();
}
