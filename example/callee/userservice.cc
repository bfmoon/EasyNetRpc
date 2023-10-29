#include<iostream>
#include<string>
#include"user.pb.h"
#include"mprpcapplication.h"
#include"rpcprovider.h"
#include<google/protobuf/service.h>
#include"loginfo.h"

/*
该文件是rpc服务提供者，供外部调用
例如，此处是添加、删除好友的操作，另外一个主机要进行添加好友，将相关调用函数序列化
通过网络发送，然后此处通过反序列化解析出要进行的操作，执行相关函数
执行完成之后，进行应答消息的返回

然而现在模拟具体业务的调用场景，未实现rpc框架
*/

class UserService:public fixbug::UserServiceRpc
{
public:
    bool Login(std::string name,std::string pwd)
    {
        std::cout<<"Login is running"<<std::endl;
        std::cout<<"name: "<<name<<std::endl;
        return true;
    }

    bool Register(int id,std::string name,std::string pwd)
    {
        std::cout<<"Register is running"<<std::endl;
        std::cout<<"id: "<<id<<"name: "<<name<<"pwd: "<<pwd<<std::endl;
        return true;
    }
    
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginRespond* response,
                       ::google::protobuf::Closure* done)
     {
        //反序化，得到姓名与密码，然后执行登录操作
        std::string name=request->name();
        std::string pwd=request->pwd();

        bool ret=Login(name,pwd);

        //返回应答消息

        fixbug::ResultCode* code=response->mutable_result();

        code->set_errcode(0);
        code->set_errmsg("");

        response->set_success(ret);

        //执行回调函数，将相应消息返回
        done->Run();
           
     }  

     void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponce* response,
                       ::google::protobuf::Closure* done)
     {
         // 反序化，得到姓名与密码，然后执行登录操作
         int id=request->id();
         std::string name = request->name();
         std::string pwd = request->pwd();

         bool ret = Register(id,name, pwd);

         // 返回应答消息

         fixbug::ResultCode *code = response->mutable_result();

         code->set_errcode(0);
         code->set_errmsg("");

         response->set_success(ret);

         // 执行回调函数，将相应消息返回
         done->Run();
     }
};



int main(int argc,char** argv)
{
    LOG_INFO("ddddd");


    //这里就是rpc框架的相关操作
    MprpcappLication::Init(argc,argv);

    RpcProvider rpc;
    rpc.NotifyService(new UserService);

    rpc.Run();

    return 0;
}