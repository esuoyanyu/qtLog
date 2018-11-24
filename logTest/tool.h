#ifndef TOOL_H
#define TOOL_H

#include <QDebug>
#include <chrono>
#include <cassert>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QFile>


#define TRIGGERLEVEL  1000 * 1000 * 800  //ms
#define DEBUG



namespace tool
{

template <typename T>
class threadSafeQueue
{
public:

    threadSafeQueue() = default;
    virtual ~threadSafeQueue(){}
    threadSafeQueue(const threadSafeQueue&) = delete;
    operator =(const threadSafeQueue&) = delete;
    threadSafeQueue(const threadSafeQueue&&) = delete;
    operator =(const threadSafeQueue&&) = delete;

    threadSafeQueue(const std::initializer_list<T> &value)
    {
        for(auto temp : value)
            item.push_back(temp);
    }

    threadSafeQueue(const T &value)
    {
        item.push_back(value);
    }


    T pop() //出队
    {
        QMutexLocker locker(&mutex);
        if(item.isEmpty())
            cond.wait(&mutex);

        auto temp = item.first();
        item.pop_front();

        return temp;
    }

    void push(const T& item) //入队
    {
        QMutexLocker locker(&mutex);
        this->item.push_back(item);
        cond.wakeAll();
    }

    bool isEmpty() //判断是否为空
    {
        QMutexLocker locker(&mutex);

        return item.isEmpty();
    }

private:
    QMutex mutex;
    QWaitCondition cond;
    QQueue<T> item;

}; //threadSafeQueue end


class timeTool
{
public:
    timeTool() : value(0), runingState(false) {}
    timeTool(const long long value,bool state = false) : value(value) ,runingState(state){}
    ~timeTool(){}
    timeTool(const timeTool &time) : timeTool(time.value,time.runingState)
    {
        lastTime = time.lastTime;
        currentTime = time.currentTime;
    }

    timeTool& operator=(const timeTool &time)
    {
        value = time.value;
        runingState = time.runingState;
        lastTime = time.lastTime;
        currentTime = time.currentTime;

        return *this;
    }

    timeTool(const timeTool&& time) : timeTool(time.value)
    {
        runingState = std::move(time.runingState);
        lastTime = time.lastTime;
        currentTime = time.currentTime;
    }

    timeTool& operator= (const timeTool &&time)
    {
        value = time.value;
        runingState = time.runingState;
        lastTime = time.lastTime;
        currentTime = time.currentTime;

        return *this;
    }

    void setStartTime()
    {
        if(runingState == false) //第一次循环
            return;

        currentTime = std::chrono::steady_clock::now();

    }

    void setEndTime()
    {
        runingState = true;

        lastTime = std::chrono::steady_clock::now();
    }

    bool getTimeDiff(const long long level) //纳秒
    {
        if(runingState == false) //第一次循环
            return true;

        value = (currentTime - lastTime).count();

#ifdef DEBUG
        qDebug() << __FILE__ << __func__ << __LINE__<< value;
        assert((value >= 0));
#endif


        return value > level;
    }

private:
    long long value;
    bool runingState;
    std::chrono::steady_clock::time_point lastTime;
    std::chrono::steady_clock::time_point currentTime;
}; //class timeTool end


}//namespace tool end

#endif // TOOL_H
