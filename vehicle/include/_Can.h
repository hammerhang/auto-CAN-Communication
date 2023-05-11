#ifndef CAN_COMMUNICATE_H
#define CAN_COMMUNICATE_H

#include <yaml-cpp/yaml.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/error.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <filesystem>
#include <unordered_map>
#include "_CanAutoAnalysis.h"
#include "_CanAutoEncapsulation.h"


// #define CAN_COMMUNICATION_DEBUG //调试开关


//CAN类
class Can : virtual public CanAutoAnalysis, virtual public CanAutoEncapsulation
{
    private:
        std::unordered_map<std::string,int32_t> fdMap; //can通信文件描述符表
        char* config_dir_path_env = std::getenv("config_path_dir");
        std::string config_path = config_dir_path_env;
        YAML::Node CONFIG = YAML::LoadFile(config_path + "/controlConfig.yaml");
        YAML::Node VEHICLE = YAML::LoadFile(config_path + "/VehicleCanInfo/" + CONFIG["Vehicle"].as<std::string>() + "/VEHICLE.yaml");
        std::vector<struct can_filter> canFilter; //报文过滤器
        std::vector<std::string> canInterface; //can接口集合
    protected:
        std::unordered_map<std::string, can_frame> canframeMap;
    public:
        ~Can();
        void Open(const std::vector<std::string>& canset);
        void Close();
        void Read(const std::string& can);
        void Write(const std::string& can, const struct can_frame& frame);
};

Can::~Can()
{
    //关闭资源
    this->Close();
}

void Can::Open(const std::vector<std::string>& canset){
    //获取can接口集合
    for(int i = 0; i < canset.size(); i++){
        this->canInterface.push_back(canset[i]);
        #ifdef CAN_COMMUNICATION_DEBUG
        std::cout << DEBUG << "(_CanCommulation): you opend " << canInterface[i] << RESET << std::endl;
        #endif
    }
    //获取报文过滤器
    this->canFilter.resize(VEHICLE["FeedBack"].size());
    for(int i = 0; i < VEHICLE["FeedBack"].size(); i++){
        this->canFilter[i].can_id = VEHICLE["FeedBack"][i].as<canid_t>();
        this->canFilter[i].can_mask = CAN_SFF_MASK;
    }
    for(int i = 0; i < canFilter.size(); i++){
        #ifdef CAN_COMMUNICATION_DEBUG
        std::cout << DEBUG << "(_CanCommunication): can filter is " << canFilter[i].can_id << RESET << std::endl;
        #endif
    }
    //为每一个can接口创建套接字
    for(int i = 0; i < canInterface.size(); i++){
        if((this->fdMap[canInterface[i]] = socket(PF_CAN,SOCK_RAW,CAN_RAW)) < 0) perror(ERROR "(_CanCommunication): socketcan open failed");
    }
    //指定 can 设备
    std::vector<struct ifreq> sockcanConf(canInterface.size());
    for(int i = 0; i < sockcanConf.size(); i++){
        strcpy(sockcanConf[i].ifr_name,canInterface[i].c_str());
        ioctl(this->fdMap[canInterface[i]],SIOCGIFINDEX,&sockcanConf[i]);
    }
    //将套接字地址与 can 绑定
    std::vector<struct sockaddr_can> Addr(canInterface.size());
    for(int i = 0; i < Addr.size(); i++){
        Addr[i].can_family = AF_CAN;
        Addr[i].can_ifindex = sockcanConf[i].ifr_ifindex;
        if(bind(this->fdMap[canInterface[i]], (struct sockaddr *)&Addr[i], sizeof(Addr[i])) < 0) perror(ERROR "(_CanCommunication): sockaddr_can bind failed" RESET);
    }
    /*can过滤器*/
    struct can_filter rfilter[canFilter.size()]; //转存为数组
    for(int i = 0; i < canFilter.size(); i++){
        rfilter[i] = canFilter[i];
    }
    for(int j = 0; j < canInterface.size(); j++){
        if(setsockopt(this->fdMap[canInterface[j]], SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)) < 0) perror(ERROR "(_CanCommunication): frame filter set err" RESET);
        //通过错误掩码可以实现对错误帧的过滤
        can_err_mask_t err_mask = ( CAN_ERR_TX_TIMEOUT | CAN_ERR_BUSOFF );
        if(setsockopt(this->fdMap[canInterface[j]], SOL_CAN_RAW, CAN_RAW_ERR_FILTER,&err_mask, sizeof(err_mask)) < 0) perror(ERROR "(_CanCommunication): frame filter set err" RESET);
    }
    

}

void Can::Close(){
    for(int i = 0; i < fdMap.size(); i++){
        if(close(this->fdMap[canInterface[i]]) < 0) perror(ERROR "(_CanCommunication): socktcan device close failed" RESET);
        else std::cout << INFO "(_CanCommunication): " << canInterface[i] << " has been closed" RESET << std::endl; 
    }
}

void Can::Read(const std::string& can){
    if(this->fdMap.find(can) == fdMap.end()) //没有相应的can接口被打开
    {
        std::cout << ERROR "(_CanCommunication): No can device name " << can << "be opend" RESET << std::endl;
        exit(1);
    }
    ssize_t byte_num = sizeof(can_frame);
    bzero(&this->canframeMap[can], byte_num); //清空接收缓存区
    if (sizeof(this->canframeMap[can]) != read(this->fdMap[can],&this->canframeMap[can],byte_num)){
        perror(ERROR "(_CanCommunication): frame read err" RESET);
        exit(1);
    }
}

void Can::Write(const std::string& can, const struct can_frame &frame){
    if(this->fdMap.find(can) == fdMap.end()) //没有相应的can接口被打开
    {
        std::cout << ERROR "(_CanCommunication): No can device name " << can << " be opend" RESET << std::endl;
        exit(1);
    }
    int byte_num = sizeof(can_frame);
    if (byte_num != write(this->fdMap[can],&frame,byte_num)){
        perror(ERROR "(_CanCommunication): frame send err" RESET);
        exit(1);
    }
}



#endif //CAN_COMMUNICATE_H