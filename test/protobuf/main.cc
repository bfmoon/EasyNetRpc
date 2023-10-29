#include"test.pb.h"
using namespace fixbug;
#include<iostream>
#include<string>

using namespace std;

int main01()
{
    GetFriendListsRespond rsp;
    ResultCode* res=rsp.mutable_result();
    res->set_errcode(0);
    
    User* user=rsp.add_friend_list();
    user->set_age(22);
    user->set_name("tongtou");
    user->set_sex(User::MAN);

    User* user2=rsp.add_friend_list();
    user2->set_age(22);
    user2->set_name("tongtou");
    user2->set_sex(User::MAN);

    cout<<rsp.friend_list_size()<<endl;



    return 0;
}


int main()
{
    LoginRequest  res;
    res.set_name("zhang san");
    res.set_pwd("123456");

    //然后序列化对象
    string str;
    if(res.SerializeToString(&str))
    {
        cout<<str.c_str()<<endl;
    }


    //然后反序列化
    LoginRequest req;
    if(req.ParseFromString(str))
    {
        cout<<req.name()<<endl;
        cout<<req.pwd()<<endl;
    }

    return 0;
}