#include"zookeeperutil.h"
#include<string>
#include<semaphore.h>
#include<iostream>


//全局的监听函数
//全局的watcher观察器   zkserver给zkclient的通知
void global_watcher(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx)
{
    if(type==ZOO_SESSION_EVENT)
    {
        //若状态为ZOO_CONNECTED_STATE，则表示客户端与服务器连接成功
        if(state==ZOO_CONNECTED_STATE)
        {
            //就是获取初始化函数时线程的信号量
            //要将其唤醒
            sem_t* sem=(sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}



ZkClient::ZkClient():m_zhandle(nullptr)
{}

ZkClient::~ZkClient()
{
    if(m_zhandle!=nullptr)
    {
        //释放资源。关闭句柄
        zookeeper_close(m_zhandle);   
    }
}


//初始化，建立连接
void ZkClient::Start()
{
    std::string ip=MprpcappLication::getInstance().getConfig().Load("zookeeperip");
    std::string port=MprpcappLication::getInstance().getConfig().Load("zookeeperport");
    std::string conndstr=ip+":"+port;

    //zookeeper环境初始化，由于初始化时只是对句柄创建了空间，并没有实质的操作
    m_zhandle=zookeeper_init(conndstr.c_str(),global_watcher,30000,nullptr,nullptr,0);
    if(nullptr==m_zhandle)
    {
        std::cout<<"zookeeper_init error"<<std::endl;
        exit(EXIT_FAILURE);
    }

    //由于zookeeper_init函数并不是直接调用完就返回的，而是多线程操作
    //所以利用信号量，用来进行线程间通信
    sem_t sem;
    sem_init(&sem,0,0);

    //将该信号量发送给对应线程中的句柄
    zoo_set_context(m_zhandle,&sem);

    //zookeeper_init初始化成功之后，该线程阻塞等待，通过回调函数进行监听
    //然后唤醒阻塞
    sem_wait(&sem);

    std::cout<<"zookeeper_init seccuss"<<std::endl;

}

//创建一个节点
void ZkClient::Create(const char *path, const char *data,int datalen,int state)
{
    char buffer[128]={0};
    int bufferlen=sizeof(buffer);
    int flag;
    //先判断该节点是否存在
    flag=zoo_exists(m_zhandle,path,0,nullptr);
    //ZNONODE表示不存在，则要进行创建
    if(flag==ZNONODE)
    {
        flag=zoo_create(m_zhandle,path,data,datalen,&ZOO_OPEN_ACL_UNSAFE,state,buffer,bufferlen);
        if(flag==ZOK)
        {
            std::cout<<"zoo_create success"<<std::endl;
        }
        else
        {
            std::cout << "flag:" << flag << std::endl;
			std::cout << "znode create error... path:" << path << std::endl;
			exit(EXIT_FAILURE);
        }
    }
}

std::string ZkClient::Getdata(const char *path)
{
    char buffer[128]={0};
    int bufferlen=sizeof(buffer);
    int flag;
    flag=zoo_get(m_zhandle,path,0,buffer,&bufferlen,nullptr);
    if(flag!=ZOK)
    {
        std::cout << "get znode error... path:" << path << std::endl;
		return "";
    }
    return buffer;

}