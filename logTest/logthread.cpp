#include "logthread.h"

logSpace::logThread *logSpace::logThread::logThis = new logSpace::logThread(LOGFILENAME,LOGFILEMAXSIZE);

logSpace::logThread::logThread(const QString &path,long long level,QObject* parent) : logFilePathAndName(path), level(level), QThread(parent)
{
    logFile.setFileName(logFilePathAndName);
    if(logFile.open(QIODevice::ReadWrite | QIODevice::Append))
    {
        removeFile();
        logStream.setDevice(&logFile);
    }
    else
    {

    }
}


void logSpace::logThread::run()
{
    while(1)
    {
        if(isFileFull())
            removeFile();

        QString log = queue.pop().dealLogItem();

#ifdef WIN32
        logStream << log << "\r\n";

#else
        logStream << log << "\n";
#endif

        logStream.flush();
        logFile.flush();
    }
}

void logSpace::logThread::removeFile()
{
    qint64 fileSize= logFile.size();
    qint64 count = static_cast<int>(fileSize * (3.0 / 4));

    copyFile(LOGFILENAME,QString::number(qptrdiff(QThread::currentThreadId())),count);


    logFile.remove();

    QFile fileTemp(QString::number(qptrdiff(QThread::currentThreadId())));
    fileTemp.rename(LOGFILENAME);

    logFile.setFileName(LOGFILENAME);
    logFile.open(QIODevice::ReadWrite | QIODevice::Append);
    logStream.setDevice(&logFile);
}


QString logSpace::logItem::dealLogItem()
{
    QString ret;
    ret.clear();


    switch (type)
    {
    case QtDebugMsg:
        ret = "Debug ";
        break;
    case QtInfoMsg:
        ret = "Info ";
        break;
    case QtWarningMsg:
        ret = "Warning ";
        break;
    case QtCriticalMsg:
        ret = "Critical ";
        break;
    case QtFatalMsg:
        break;
    }

    ret = ret + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + " " + msg;

    return ret;
}


bool logSpace::copyFile(const QString &src,const QString &dest,qint64 where)
{
    bool ret = true;
    QFile srcFile(src);
    ret = srcFile.open(QIODevice::ReadOnly);
    if(!ret)
        return  ret;

    if(where >= srcFile.size())
        return false;

    QFile destFile(dest);
    ret = destFile.open(QIODevice::WriteOnly | QIODevice::Append);
    if(!ret)
        return  ret;

    QTextStream srcStream(&srcFile);
    ret = srcStream.seek(where);
    if(!ret)
        return false;
    srcStream.readLine(); //读取一行 扔掉

    QTextStream destStream(&destFile);

    while(!srcStream.atEnd())
    {
        QString srcStr = srcStream.readLine();

#ifdef WIN32
        destStream << srcStr << "\r\n";

#else
        destStream << srcStr << "\n";
#endif
    }

    destStream.flush();
    destFile.flush();

    srcFile.close();
    destFile.close();

    return true;
}

