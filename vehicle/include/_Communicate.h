#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include "_Can.h"
#include "_Ethnet.h"

#define CAN "CAN"
#define ETH "ETH"

/* 通信类 : Can, Ethnet*/
class Communicate : virtual public Can, virtual public Ethnet
{
    private:
        std::set<std::string> TYPE_MAP = {CAN, ETH}; //合法的种类表
        std::string TYPE = "NULL"; //通信接口种类,初始化为"NULL"
    public:
        Communicate();
    public:
        void SetCommunicateType(std::string _TYPE);
        void Open(const std::vector<std::string>& canset); //打开CAN口
        void Open(); //打开网口
        void Close();
        can_frame Read(const std::string& can); //读取CAN口
        void Read(); //读取网口
        void Write(const std::string& can, const struct can_frame& frame); //写CAN口
        void Write(); //写网口

};

Communicate::Communicate()
{
}


void Communicate::SetCommunicateType(std::string _TYPE){
    if(TYPE_MAP.find(_TYPE) == TYPE_MAP.end()){
        std::cout << ERROR << "(_Communication SetCommunicateType): Invalid interface type" << RESET << std::endl;
        exit(-1);
    }
    this->TYPE = _TYPE;
}

void Communicate::Open(const std::vector<std::string>& canset){
    if(this->TYPE == "NULL"){
        std::cout << ERROR << "(_Communication Open): You should set communicate type first" << RESET << std::endl;
        exit(-1);
    }
    else if(this->TYPE != CAN){
        std::cout << ERROR << "(_Communication Open): The interface type is not CAN" << RESET << std::endl;
        exit(-1);
    }
    Can::Open(canset);
}

void Communicate::Open(){
    if(this->TYPE != ETH){
        std::cout << ERROR << "(_Communication Open): The interface type is not ETH" << RESET << std::endl;
        exit(-1);
    }
    Ethnet::Open();
}

void Communicate::Close(){
    if(this->TYPE == CAN)
        Can::Close();
    if(this->TYPE == ETH)
        Ethnet::Close();
}

can_frame Communicate::Read(const std::string& can){
    if(this->TYPE != CAN){
        std::cout << ERROR << "(_Communication Read): The interface type is not CAN" << RESET << std::endl;
        exit(-1);
    }
    Can::Read(can);
    return Can::canframeMap[can];
}

void Communicate::Read(){
    if(this->TYPE != ETH){
        std::cout << ERROR << "(_Communication Read): The interface type is not ETH" << RESET << std::endl;
        exit(-1);
    }
    Ethnet::Read();
}

void Communicate::Write(const std::string& can, const struct can_frame& frame){
    if(this->TYPE != CAN){
        std::cout << ERROR << "(_Communication Write): The interface type is not CAN" << RESET << std::endl;
        exit(-1);
    }
    Can::Write(can, frame);
}

void Communicate::Write(){
    if(this->TYPE != ETH){
        std::cout << ERROR << "(_Communication Write): The interface type is not ETH" << RESET << std::endl;
        exit(-1);
    }
    Ethnet::Write();
}


#endif