#pragma once
#include<zookeeper/zookeeper.h>
#include"mprpcapplication.h"

class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    //开启zookeeper
    void Start();
    //创建节点
    void Create(const char *path, const char *data,int datalen,int state=0);

    //获取对应节点的值
    std::string Getdata(const char *path);

private:
    zhandle_t* m_zhandle;
};