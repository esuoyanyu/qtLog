#ifndef LOGTHREAD_H
#define LOGTHREAD_H
#include "tool.h"
#include <QThread>
#include <QDateTime>
#include <QFileInfo>
#include <QDateTime>

#define LOGFILENAME "product_test_log"
#define LOGFILEMAXSIZE 0x8fffff  //8MB

namespace logSpace
{

bool copyFile(const QString &src,const QString &dest,qint64 where = 0);

struct logItem
{
    logItem() = default;
    logItem(const QtMsgType& type,const QString& msg):type(type),msg(msg){}
    logItem(const logItem& item) : logItem(item.type,item.msg){}
    logItem& operator =(const logItem& item) { type = item.type;  msg = item.msg; return *this;}
    logItem(const logItem&& item) : logItem(item.type,item.msg){}
    logItem& operator=(const logItem&& item){ type = item.type; msg = item.msg; return *this;}
    ~logItem(){}

    QString dealLogItem();

private:
    QtMsgType type;
    QString msg;
};

class logThread : public QThread
{
    Q_OBJECT

public:
    logThread(const logThread&) = delete;
    logThread& operator =(const logThread&) = delete;
    logThread(const logThread &&) = delete;
    logThread & operator =(const logThread&&) = delete;

    void addDateToQueue(const logItem& value) {queue.push(value);}
    static logThread* getInterface(){return logThis;}
    static void destroyInterface(){logThis->~logThread();}

    void removeFile();



protected:
    void run() override;

private:
    QString logFilePathAndName;
    long long level;
    QFile logFile;
    QTextStream logStream;
    static logThread* logThis;
    tool::threadSafeQueue<logItem> queue;
    logThread(QObject* parent = nullptr) : QThread(parent){}
    logThread(const QString &path = QString("log.txt"),long long level = 0x40fffff/*64MB*/,QObject* parent = nullptr);
    ~logThread(){logFile.close(); delete logThis; logThis = nullptr;}

    bool isFileFull() {return logFile.size() >= level;}

}; //class logThread end
} //namespace log end


#endif // LOGTHREAD_H
