#include"mprpccontroller.h"

MprpcController::MprpcController()
{
    m_fail=false;
    m_errText="";
}
void MprpcController::Reset()
{
     m_fail=false;
     m_errText="";
}
bool MprpcController::Failed() const
{
    return m_fail;
}
std::string MprpcController::ErrorText() const
{
    return m_errText;
}

void MprpcController::SetFailed(const std::string &reason)
{
    m_fail=true;
    m_errText=reason;
}

void MprpcController::StartCancel(){}
bool MprpcController::IsCanceled() const {return false;}
void MprpcController::NotifyOnCancel(google::protobuf::Closure *callback) {}