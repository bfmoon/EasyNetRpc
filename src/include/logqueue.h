#pragma once

#include<queue>
#include<mutex>
#include<condition_variable>
#include<thread>

//实现日志队列，涉及线程安全，线程间通信问题

template<typename T>
class LogQueue
{
public:
    void push(const T& data)
    {
        //加锁
        std::lock_guard<std::mutex>lock(m_mutex);
        //然后往队列中放入元素
        m_que.push(data);

        //在插入成功之后应该进行唤醒操作，唤醒其他阻塞等待的线程
        m_condvariable.notify_one();
    }

    T pop()
    {
        //加锁
        std::unique_lock<std::mutex>lock(m_mutex);

        //然后判断队列是否为空，若为空则应该进行阻塞等待，但是要释放锁
        while(m_que.empty())
        {
            m_condvariable.wait(lock);
        }
        //不为空时，进行弹出操作
        T data=m_que.front();
        m_que.pop();

        return data;
    }
private:
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
    std::queue<T> m_que;
};