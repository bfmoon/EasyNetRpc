#pragma once
#include<google/protobuf/service.h>


//RpcController表示控制信息，记录是否产生错误
class MprpcController:public google::protobuf::RpcController
{
public:
    MprpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);

    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);

private:
    bool m_fail;
    std::string m_errText;
};