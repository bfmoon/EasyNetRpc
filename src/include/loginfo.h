#pragma once

#include"logqueue.h"
#include<string>



// 定义宏 LOG_INFO("xxx %d %s", 20, "xxxx")
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        LogInfo &logger = LogInfo::getInstance(); \
        logger.setLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.setLog(c); \
    } while(0) \

#define LOG_ERR(logmsgformat, ...) \
    do \
    {  \
        LogInfo &logger = LogInfo::getInstance(); \
        logger.setLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.setLog(c); \
    } while(0) \


enum LOG_LEVEL
{
    INFO,   //普通日志信息
    ERROR,    //错误日志信息
};

//日志类可用单例模式实现
class LogInfo
{
public:
    //获取单例对象
    static LogInfo& getInstance();
    //设置日志级别
    void setLevel(LOG_LEVEL level);

    //进行写日志操作
    void setLog(std::string msg);
private:
    LogInfo();
    LogInfo(const LogInfo&)=delete;
    LogInfo(LogInfo&&)=delete;
private:
    LOG_LEVEL m_level;//日志的级别
    LogQueue<std::string> m_logque;//日志队列

};