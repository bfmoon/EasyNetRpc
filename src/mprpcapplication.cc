#include"mprpcapplication.h"
#include<iostream>
#include<string>
#include<unistd.h>


MprpcConfig MprpcappLication::m_config;
//显示具体操作命令
void showInfo()
{
    std::cout<<"format: command -i <configfile>" << std::endl;
}


//初始化环境，通过配置文件获取到ip与端口
void MprpcappLication::Init(int argc,char**argv)
{
    if(argc<2)
    {
        showInfo();
        exit(EXIT_FAILURE);
    }
    //判断命令参数是否合法
    int c=0;
    std::string config_str;
    while((c=getopt(argc,argv,"i:"))!=-1)
    {
        switch (c)
        {
        case 'i':
            //如果刚好是i，则后面跟的就是文件名
            config_str=optarg;
            break;
        case '?':
            showInfo();
            exit(EXIT_FAILURE);

        case ':':
            showInfo();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    //获取文件名之后，就要对文件进行解析处理
    //获取对应ip与端口
    m_config.LoadConfigFile(config_str.c_str());

}

MprpcappLication& MprpcappLication::getInstance()
{
    static MprpcappLication app;
    return app;
}

MprpcConfig& MprpcappLication::getConfig()
{
    return m_config;
}