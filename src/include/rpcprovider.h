#pragma once
#include<iostream>
#include<google/protobuf/service.h>
#include<muduo/net/TcpConnection.h>
#include<muduo/net/TcpServer.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/EventLoop.h>
#include<string>
#include<functional>
#include<unordered_map>



class RpcProvider
{
public:
    //提供服务，利用基类指针，可以指向任意派生类对象
    void NotifyService(google::protobuf::Service* service);
    //启动rpc网络通信，所以就要涉及到网络通信
    //利用muduo库完成
    void Run();
private:
    void OnConnect(const muduo::net::TcpConnectionPtr&);
    void OnMessage(const muduo::net:: TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);

    //设置请求方要执行方法的函数回调
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&,google::protobuf::Message*);

private:
    //定义一个结构，保存服务对象以及对象下面的方法，也就是函数名和函数的具体描述
    struct ServiceInfo
    {
        //定义服务对象
        google::protobuf::Service* m_service;
        //函数名和函数的具体描述
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*>m_methodMap; 
    };
    //保存类以及对应的ServiceInfo
    std::unordered_map<std::string,ServiceInfo>m_serviceMap;


    muduo::net::EventLoop m_evenLoop;
};