#pragma once
#include<iostream>
#include<unordered_map>
#include<string>
//加载配置文件类
class MprpcConfig
{
public:
    //解析配置文件，将获取到的ip，port放入map中
    void LoadConfigFile(const char* config_file);
    //通过key,获取对应的value
    std::string Load(const std::string&key);

private:
    //处理字符串前后的空格
    void Trim(std::string&src_buf);
    std::unordered_map<std::string,std::string>m_configMap;

};