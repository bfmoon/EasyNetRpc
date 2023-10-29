#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
int main(int argc, char **argv)
{
    // 初始化环境
    MprpcappLication::Init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    // stub需要一个RpcChannel类型的参数，但是RpcChannel类是一个虚基类，无法实例化对象
    // 所以要rpc框架要重写该类
    /*
    void CallMethod(const MethodDescriptor* method,
                          RpcController* controller, const Message* request,
                          Message* response, Closure* done) = 0;
    */
    // 定义请求数据
    fixbug::RegisterRequest req;
    req.set_id(66);
    req.set_name("tongtou");
    req.set_pwd("666666");

    fixbug::RegisterResponce res;
     MprpcController contro;
    // stub.Login(nullptr, &request, &responce, nullptr);
    stub.Register(&contro, &req, &res, nullptr);
    if(contro.Failed())
    {
        std::cout<<contro.ErrorText()<<std::endl;
    }
    else
    {
         // 一次rpc调用完成，读调用的结果
         if (0 == res.result().errcode())
         {
             std::cout << "rpc Register response success:" << res.success() << std::endl;
         }
         else
         {
             std::cout << "rpc Register response error : " << res.result().errmsg() << std::endl;
         }
    }
   
    return 0;
}