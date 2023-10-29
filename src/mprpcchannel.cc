#include"mprpcchannel.h"
#include"rpcheader.pb.h"
#include"mprpcapplication.h"
#include<string>
#include<iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
#include"loginfo.h"
#include"zookeeperutil.h"

//这里是请求方发送的请求，就是将数据序列化以及网络发送的相关操作
void MprpcChannel::CallMethod(const google::protobuf:: MethodDescriptor* method,
                              google::protobuf::RpcController* controller, 
                              const google::protobuf::Message* request,
                              google::protobuf:: Message* response, 
                              google::protobuf::Closure* done)
{
     const google::protobuf::ServiceDescriptor*sd=method->service();
     //获取要求服务的类名
     std::string service_name=sd->name();
     //获取要求服务的方法名
     std::string method_name=method->name();

     //获取序列化后的参数的长度，proto文件中已经定义
     uint32_t agrs_size=0;
     std::string args_str;

     if(request->SerializeToString(&args_str))
     {
        agrs_size=args_str.size();
     }
     else
     {
        controller->SetFailed("serialize request error!");
        return ;
     }

     //组织数据头格式
     mprpc::RpcHeader rpcHeader;
     rpcHeader.set_service_name(service_name);
     rpcHeader.set_method_name(method_name);
     rpcHeader.set_args_size(agrs_size);

     //设置数据头的长度
     uint32_t header_size=0;
     std::string header_str;
     if(rpcHeader.SerializeToString(&header_str))
     {
        header_size=header_str.size();
     }
     else
     {
        //std::cout<<"serialize rpcHeader error!"<<std::endl;
        controller->SetFailed("serialize rpcHeader error!");
        return ;
     }

     //然后组织数据格式
     //header_size+header_str+args_size
     std::string send_str;
     send_str.insert(0,std::string((char*)&header_size,4));
     send_str+=header_str;
     send_str+=args_str;

        // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout<<"rpcchannel.cc"<<std::endl;
    std::cout << "rpc_header_str: " << header_str << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl;  
    std::cout << "args_str: " << args_str << std::endl; 
    std::cout << "============================================" << std::endl;

     //然后就是将数据发送
     //利用tcp发送
    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    if (-1 == clientfd)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    //获取ip和端口
    ZkClient zkCli;
    zkCli.Start();
    std::string path="/"+service_name+"/"+method_name;
    std::string hosts_data=zkCli.Getdata(path.c_str());
     if (hosts_data == "")
    {
        controller->SetFailed(path + " is not exist!");
        return;
    }
    int idx = hosts_data.find(":");
    if (idx == -1)
    {
        controller->SetFailed(path + " address is invalid!");
        return;
    }
    std::string ip=hosts_data.substr(0,idx);
    uint16_t port=atoi(hosts_data.substr(idx+1,hosts_data.size()-idx).c_str());

    //std::string ip=MprpcappLication::getInstance().getConfig().Load("rpcserverip");
    //uint16_t port=atoi(MprpcappLication::getInstance().getConfig().Load("rpcserverport").c_str());
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=inet_addr(ip.c_str());
    int ret=connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(-1==ret)
    {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    //然后发送
    if(-1==send(clientfd,send_str.c_str(),send_str.size(),0))
    {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    //接收发送过来的数据
    char recv_buf[1024]={0};
    int recv_size=recv(clientfd,recv_buf,1024,0);
    if(-1==recv_size)
    {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    //反序列化接收过来的数据
    //由于string构造函数遇到\n时就自动结束，所以responce_str只保存了\n,所以出错了
    //std::string responce_str(recv_buf,0,recv_size);
    //所以直接对数组进行反序列化

    //if(!response->ParseFromString(responce_str))
    if(!response->ParseFromArray(recv_buf,recv_size))
    {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "Parse error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }
    close(clientfd); 
}