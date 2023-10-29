#include"mprpcconfig.h"

//解析配置文件，将获取到的ip，port放入map中
void MprpcConfig::LoadConfigFile(const char* config_file)
{
    //打开文件
    FILE* pf=fopen(config_file,"r");
    if(nullptr==pf)
    {
        std::cout << config_file << " is note exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    //读取文件，一直到文件末尾
    while(!feof(pf))
    {
        char buf[128]={0};
        //一次读取一行
        fgets(buf,128,pf);

        //将char*转换为string,方便处理
        std::string read_buf(buf);

        //处理不符合配置文件规则的选项
        //处理字符串前后的空格
        Trim(read_buf);

        //处理之后，查找配置项
        //如果是#号，说明是注释，跳过
        if(read_buf[0]=='#'||read_buf.empty())
        {
            continue;
        }

        int idx=read_buf.find('=');
        if(idx!=-1)
        {
            std::string key;
            std::string value;
            key=read_buf.substr(0,idx);
            Trim(read_buf);
            int endIdx=read_buf.find('\n');
            //截取value
            value=read_buf.substr(idx+1,endIdx-idx-1);
            Trim(read_buf);

            m_configMap.insert({key,value});
        }
        
    }

    fclose(pf);
}
//通过key,获取对应的value
std::string MprpcConfig::Load(const std::string&key)
{
    auto it=m_configMap.find(key);
    if(it==m_configMap.end())
    {
        return "";
    }
    return it->second;
}

void MprpcConfig::Trim(std::string&src_buf)
{
    //找到该字符串，第一个不是空格的字符
    int idx=src_buf.find_first_not_of(' ');
    if(idx!=-1)
    {
        //截取，直接截取后面字符串即可
        src_buf=src_buf.substr(idx,src_buf.size()-idx);
    }
    //然后截取字符串后面的空格
    idx=src_buf.find_last_of(' ');
    if(idx!=-1)
    {
        src_buf=src_buf.substr(0,idx+1);
    }
}