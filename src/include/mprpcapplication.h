#pragma once
#include<iostream>
#include<string>
#include"mprpcconfig.h"

class MprpcappLication
{
public:
    static void Init(int argc,char**argv);

    static MprpcappLication& getInstance();
    static MprpcConfig& getConfig();
private:
    MprpcappLication(){}
    MprpcappLication(const MprpcappLication&)=delete;
    MprpcappLication(MprpcappLication&&)=delete;
private:
    static MprpcConfig m_config;
};