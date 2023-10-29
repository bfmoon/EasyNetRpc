#include"rpcprovider.h"
#include"mprpcapplication.h"
#include"rpcheader.pb.h"
#include<google/protobuf/descriptor.h>
#include"loginfo.h"
#include"zookeeperutil.h"

//框架供外部使用，发布rpc方法
//由于每个模块会有很多的函数方法，但是在rpc中不知道具体调用哪一个
//所以就要用一个表记录每个对象下的每一个方法
//例如，UserService下有Login,和Res方法，所以要在表中记录相关的
//类名，一个类下面的方法和函数
void RpcProvider::NotifyService(google::protobuf::Service* service)
{
    //先定义一个方法与对象的具体信息对象
    ServiceInfo service_info;
    //获取服务对象的描述信息，也就是举例中的UserService的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc=service->GetDescriptor();
    //获取描述信息中的类名，UserService
    std::string service_name=pserviceDesc->name();
    //std::cout<<"service_name: "<<service_name<<std::endl;
    LOG_INFO("service_name: %s",service_name);
    //获取该类下面有多少中方法，也就是函数
    const int methodCnt=pserviceDesc->method_count();
    //std::cout<<"methodCnt: "<<methodCnt<<std::endl;
    LOG_INFO("methodCnt: %d",methodCnt);
    for(int i=0;i<methodCnt;++i)
    {
        //获取类中每个方法的描述信息，也就是UserService中函数的描述信息
        const google::protobuf::MethodDescriptor* pmethodDesc=pserviceDesc->method(i);
        //获取方法名,函数名
        std::string method_name=pmethodDesc->name();
        //std::cout<<"method_name: "<<method_name<<std::endl;
        LOG_INFO("method_name: %s",method_name);
        service_info.m_methodMap.insert({method_name,pmethodDesc});
    }
    service_info.m_service=service;
    m_serviceMap.insert({service_name,service_info});
}

//开启rpc网络服务，涉及到网络通信
void RpcProvider::Run()
{
    std::string ip=MprpcappLication::getInstance().getConfig().Load("rpcserverip");
    uint16_t port=atoi(MprpcappLication::getInstance().getConfig().Load("rpcserverport").c_str());
    
    muduo::net::InetAddress addr(ip,port);
    
    //创建muduo中server对象
    muduo::net::TcpServer server(&m_evenLoop,addr,"RpcProvider");

    //设置回调函数
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnect,this,std::placeholders::_1));

    server.setMessageCallback(std::bind(&RpcProvider::OnMessage,this,
            std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    
    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    // session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
    ZkClient zkCli;
    zkCli.Start();
    //记录每个方法，也就是要请求的函数
    for(auto& sp:m_serviceMap)
    {
        std::string service_path="/"+sp.first;
        zkCli.Create(service_path.c_str(),nullptr,0);
        for(auto&mp :sp.second.m_methodMap)
        {
            std::string method_parh=service_path+"/"+mp.first;
            char databuf[128]={0};
            sprintf(databuf,"%s:%d",ip.c_str(),port);
            zkCli.Create(method_parh.c_str(),databuf,sizeof(databuf),ZOO_EPHEMERAL);
        }
    }

    //设置线程数量
    server.setThreadNum(4);
    std::cout<<"RpcSeivrce start at ip"<<ip<<"port "<<port<<std::endl;
    //LOG_INFO("RpcSeivrce start at ip: %s",ip,"port: %d ",port);
    server.start();
    m_evenLoop.loop();

}

//处理连接事件的回调函数
void RpcProvider::OnConnect(const muduo::net::TcpConnectionPtr&conn)
{
    if(!conn->connected())
    {
        conn->shutdown();
    }
}

//处理收发消息的事件回调
/*
当从请求方发出数据，要通过一个协议规定发送数据的格式，
例如：UserServiceLoginzhang san123456
就是在UserService类中有个Login函数，参数为zhang san 密码为123456
但是请求方如果直接发送该数据，服务方无法解析该数据，因为不知道应该到哪里才是一个完整的
所以就要加上包头，用一个int型数据表示下一个完整的名称的长度
16UserServiceLoginzhang san123456
header_size+header_str+args_str
*/
void RpcProvider::OnMessage(const muduo::net:: TcpConnectionPtr&conn,muduo::net::Buffer* buffer,muduo::Timestamp time)
{
    //将发送过来的消息反序列化为字符串
    std::string read_buf=buffer->retrieveAllAsString();
    //然后解析该字符串，找到请求的函数方法
    uint32_t header_size=0;
    //利用string中copy方法，将前四个字节的整数取出
    read_buf.copy((char*)&header_size,4,0);

    //截取相关方法的名称，然后再进行反序列化解析
    std::string header_str=read_buf.substr(4,header_size);
    //由protobuf定义的类，规定了一些变量的反序列化
    mprpc::RpcHeader rpcHeader;
    
    std::string service_name;
    std::string method_name;
    uint32_t args_size=0;

    //将方法名称进行反序列化
    if(rpcHeader.ParseFromString(header_str))
    {
        //如果反序列化成功，则将相关数据取出
        service_name=rpcHeader.service_name();
        method_name=rpcHeader.method_name();
        args_size=rpcHeader.args_size(); 
    }
    else
    {

        std::cout<<"rpc_header_str:" << header_str << " parse error!" << std::endl;
        //LOG_ERR("rpc_header_str: %s",header_str,"parse error!");
        return;
    }
    std::cout<<header_str<<std::endl;
    //然后根据获得的参数长度，截取出相关参数
    std::string args_str=read_buf.substr(4+header_size,args_size);

    //打印日志
    std::cout<<"======================================"<<std::endl;
    std::cout<<"rpcprovider.cc"<<std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "rpc_header_str: " << header_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_str: " << args_str << std::endl; 
    std::cout<<"======================================"<<std::endl;

    //然后根据获得的参数从表中取出对应的对象
    auto it=m_serviceMap.find(service_name);
    if(it==m_serviceMap.end())
    {
        //打印日志
        std::cout<<service_name << " is not exist!" << std::endl;
       // LOG_ERR("%s",service_name," is not exist!");
        return ;
    }
    //根据获得的对象取出类内的相关方法
    auto mit=it->second.m_methodMap.find(method_name);
    if(mit==it->second.m_methodMap.end())
    {
        //打印日志
        std::cout<<method_name << " is not exist!" << std::endl;
       // LOG_ERR("%s",method_name," is not exist!");
        return ;
    }
    //获得相应的对象
    google::protobuf::Service* service=it->second.m_service;

    //获取对应的方法，Login
    const google::protobuf::MethodDescriptor*method=mit->second;

    //由rpc设置请求参数，request的相关参数，以及返回相应的响应responce
    //由于protobuf生成message相关信息，都是继承google中message的
    //所以用基类指针就可以访问任意派生类中的请求参数
    google::protobuf::Message* request=service->GetRequestPrototype(method).New();
    //设置号请求参数之后，进行反序列化，解析请求参数
    if(!request->ParseFromString(args_str))
    {
         //打印日志
        std::cout<<args_str << " is not exist!" << std::endl;
       // LOG_ERR("%s",method_name," is not exist!");
        return ;
    }
    //设置应答参数
    google::protobuf::Message* responce=service->GetResponsePrototype(method).New();

    /*
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginRespond* response,
                       ::google::protobuf::Closure* done)
    设置该函数中done的回调函数
    */
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider,
                                                                    const muduo::net::TcpConnectionPtr&, 
                                                                    google::protobuf::Message*>
                                                                    (this,&RpcProvider::SendRpcResponse,conn,responce);                                      
    // 执行请求方发送过来的函数请求
    service->CallMethod(method, nullptr, request, responce, done);
}

 //设置请求方要执行方法的函数回调
 //这里就是服务方执行完后，将应答消息序列化之后，发送给请求方
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr&conn,google::protobuf::Message*responce)
{
    std::string responce_str;
    if(responce->SerializeToString(&responce_str))
    {
        //说明序列化成功，然后发送给请求方
        conn->send(responce_str);
    }
    else
    {
        std::cout<<"serialize response_str error!" << std::endl;
        return ;
    }
    conn->shutdown();//仿照http请求，进行短链接，当服务完成之后，服务端关闭连接.让更过资源供其他用户使用
}