#include "loginfo.h"
#include<thread>
#include<time.h>
#include<iostream>

//构造函数中开启另一个线程进行写日志操作
LogInfo::LogInfo()
{
    //设置一个线程，专门从日志队列中读取该日志
    std::thread workthread([&](){
        for(;;)
        {
            //获取当前时间，返回从1970年到当前的秒数
            time_t now=time(nullptr);

            tm* cur_time=localtime(&now);

            //设置要写入日志的文件名
            char buf[128]={0};
            sprintf(buf,"%d-%d-%d-log.txt",cur_time->tm_year+1900,cur_time->tm_mon+1,cur_time->tm_mday);
            //打开该文件
            FILE* pf=fopen(buf,"a+");
            if(pf==nullptr)
            {
                std::cout<<"this file not "<<std::endl;
                exit(EXIT_FAILURE);
            }
            
            //获取日志信息
            std::string msg=m_logque.pop();
            //然后追加日志的时间戳
            char time_buf[128]={0};
            sprintf(time_buf,"%d:%d:%d =>[%s]",
                                    cur_time->tm_hour,
                                    cur_time->tm_min,
                                    cur_time->tm_sec,
                                    (m_level==INFO?"info":"error"));
            msg.insert(0,time_buf);
            msg.append("\n");

            //然后追加数据
            fputs(msg.c_str(),pf);

            fclose(pf);
        }

    });
    //设置为分离线程---守护线程
    workthread.detach();

}

// 获取单例对象
LogInfo &LogInfo::getInstance()
{
    static LogInfo info;
    return info;
}
// 设置日志级别
void LogInfo::setLevel(LOG_LEVEL level)
{
    m_level=level;
}

// 进行写日志操作
void LogInfo::setLog(std::string msg)
{
    m_logque.push(msg.c_str());
}