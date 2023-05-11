/*详情：......*/

#ifndef CAN_INFO_H
#define CAN_INFO_H

// #define CAN_INFO_DEBUG //调试开关

#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>
#include <unordered_map>
// #include <ros/package.h>
#include "_DBCAnalysis.h"
#include "_Msg.h"

typedef struct isDBC{
    bool is = false;
    std::string path;
} isDBC_t;

typedef struct Heart{
    int max;
    int min;
} Heart_t;

typedef struct Check{
    std::vector<uint16_t> byte;
    std::string mode;
} Check_t;

typedef std::unordered_map<uint32_t, std::list<CanSignal_t>> ACanSignalMap_t;
typedef std::unordered_map<uint32_t, std::list<CanSignal_t>> ECanSignalMap_t;
typedef std::unordered_map<std::string, Heart_t> HeartMap_t;
typedef std::unordered_map<std::string, Check_t> CheckMap_t;

/*获取CAN报文信息类*/
class CanInfo : virtual public DBCAnalysis
{
    private:
        char* config_dir_path_env = std::getenv("config_path_dir");
        std::string config_path = config_dir_path_env;
        YAML::Node CONFIG = YAML::LoadFile(config_path + "/controlConfig.yaml");
        YAML::Node VEHICLE = YAML::LoadFile(config_path + "/VehicleCanInfo/" + CONFIG["Vehicle"].as<std::string>() + "/VEHICLE.yaml");
        YAML::Node ACAN;
        YAML::Node ECAN;
        isDBC_t isDBC;
    private:
        void FaultTolerance(const YAML::Node& node);
    private:
        ACanSignalMap_t ACanSignalMap;
        ECanSignalMap_t ECanSignalMap;
        HeartMap_t HeartSignalMap;
        CheckMap_t CheckSignalMap;
    protected:
        ACanSignalMap_t GetACanSignalMap();
        ECanSignalMap_t GetECanSignalMap();
        HeartMap_t GetHeartSignalMap();
        CheckMap_t GetCheckSignalMap();
    public:
        CanInfo();
    protected:
        void ACanSignalMapInit();
        void ECanSignalMapInit();
};

CanInfo::CanInfo(){
    //容错检查：检查CONFIG.yaml是否有DBC、Path、Control、FeedBack这几个必要的key
    FaultTolerance(VEHICLE);
    //获取心跳信号信息
    for(int i = 0; i < VEHICLE["Heart"].size(); ++i){
        std::string signalname = VEHICLE["Heart"][i]["SignalName"].as<std::string>();
        HeartSignalMap[signalname].min = VEHICLE["Heart"][i]["scope"][0].as<int>();
        HeartSignalMap[signalname].max = VEHICLE["Heart"][i]["scope"][1].as<int>();
    }
    //获取校验信号信息
    for(int i = 0; i < VEHICLE["Check"].size(); ++i){
        for(int j = 0; j < VEHICLE["Check"][i]["byte"].size(); ++j)
            CheckSignalMap[VEHICLE["Check"][i]["SignalName"].as<std::string>()].byte.push_back(VEHICLE["Check"][i]["byte"][j].as<uint16_t>());
        CheckSignalMap[VEHICLE["Check"][i]["SignalName"].as<std::string>()].mode = VEHICLE["Check"][i]["mode"].as<std::string>();
    }
    //获取DBC相关信息
    if(this->VEHICLE["DBC"]["Status"].as<std::string>() == "ON"){
        this->isDBC.is = true;
        this->isDBC.path = config_path + "/VehicleCanInfo/" + CONFIG["Vehicle"].as<std::string>() + "/" + VEHICLE["DBC"]["Name"].as<std::string>();
        DBCAnalysis::getDBC(this->isDBC.path); //获取DBC信息
        #ifdef CAN_INFO_DEBUG
            std::cout << DEBUG "(_CanInfo): intance has a DBC file, name is <" << VEHICLE["DBC"]["Path"] << ">" RESET << std::endl;
        #endif
    }
#ifdef CAN_INFO_DEBUG
    else{
        std::cout << DEBUG "(_CanInfo): intance don't has DBC file" RESET << std::endl;
    }
#endif
}

ACanSignalMap_t CanInfo::GetACanSignalMap(){
    return this->ACanSignalMap;
}

ECanSignalMap_t CanInfo::GetECanSignalMap(){
    return this->ECanSignalMap;
}

HeartMap_t CanInfo::GetHeartSignalMap(){
    return this->HeartSignalMap;
}

CheckMap_t CanInfo::GetCheckSignalMap(){
    return this->CheckSignalMap;
}

void CanInfo::ACanSignalMapInit(){
    CanSignal_t tmp_signal;
    uint32_t ID;
    if(this->isDBC.is){
        for(int i = 0; i < VEHICLE["FeedBack"].size(); i++){
            ID = VEHICLE["FeedBack"][i].as<uint32_t>(); //拿到CONFIG.yaml文件中的反馈报文ID
            for(std::list<CanSignal_t>::iterator it = this->DBCAnalysis::CanMessageMap[ID].SignalList.begin(); it != this->DBCAnalysis::CanMessageMap[ID].SignalList.end(); ++it){
                ACanSignalMap[ID].push_back(*it);
            }
        }
    }
    else{
        ACAN = YAML::LoadFile(config_path + "/VehicleCanInfo/" + CONFIG["Vehicle"].as<std::string>() + "/ACAN.yaml");
        std::string IDstr, Sigstr;
        //插入ACANSignalMap
        for(YAML::Node::const_iterator it = ACAN.begin(); it != ACAN.end(); it++){
            ID = it->first.as<uint32_t>();
            IDstr = it->first.as<std::string>();
            for(YAML::Node::const_iterator it2 = ACAN[IDstr].begin(); it2 != ACAN[IDstr].end(); it2++){
                Sigstr = it2->first.as<std::string>();
                tmp_signal.Name = Sigstr;
                if(ACAN[IDstr][Sigstr]["StartBit"].IsDefined())
                    tmp_signal.StartBit = ACAN[IDstr][Sigstr]["StartBit"].as<uint16_t>();
                if(ACAN[IDstr][Sigstr]["Length"].IsDefined())
                    tmp_signal.Length = ACAN[IDstr][Sigstr]["Length"].as<uint16_t>();
                if(ACAN[IDstr][Sigstr]["ByteOrder"].IsDefined())
                    tmp_signal.ByteOrder = ACAN[IDstr][Sigstr]["ByteOrder"].as<std::string>();
                if(ACAN[IDstr][Sigstr]["ValueType"].IsDefined())
                    tmp_signal.ValueType = ACAN[IDstr][Sigstr]["ValueType"].as<std::string>();
                if(ACAN[IDstr][Sigstr]["Factor"].IsDefined())
                    tmp_signal.Factor = ACAN[IDstr][Sigstr]["Factor"].as<float>();
                if(ACAN[IDstr][Sigstr]["Offset"].IsDefined())
                    tmp_signal.Offset = ACAN[IDstr][Sigstr]["Offset"].as<uint16_t>();
                ACanSignalMap[ID].push_back(tmp_signal);
            }
        }
    }
    #ifdef CAN_INFO_DEBUG
        for(auto it : this->ACanSignalMap){
            std::cout << "ID:" << it.first << std::endl;
            for(auto ic : it.second){
                std::cout << ic.Name << " ";
                std::cout << ic.StartBit << " ";
                std::cout << ic.Length << " ";
                std::cout << ic.ByteOrder << " ";
                std::cout << ic.ValueType << " ";
                std::cout << ic.Factor << " ";
                std::cout << ic.Offset << std::endl;
            }
        }
    #endif
}

void CanInfo::ECanSignalMapInit(){
    std::string IDstr, Sigstr;
    uint32_t ID;
    //插入ECANSignalMap
    if(this->isDBC.is){
        for(int i = 0; i < VEHICLE["Control"].size(); i++){
            ID = VEHICLE["Control"][i].as<uint32_t>(); //拿到CONFIG.yaml文件中的控制报文ID
            for(std::list<CanSignal_t>::iterator it = this->DBCAnalysis::CanMessageMap[ID].SignalList.begin(); it != this->DBCAnalysis::CanMessageMap[ID].SignalList.end(); ++it){
                ECanSignalMap[ID].push_back(*it);
            }
        }
    }
    else{
        ECAN = YAML::LoadFile(config_path + "/VehicleCanInfo/" + CONFIG["Vehicle"].as<std::string>() + "/ECAN.yaml");
        for(YAML::const_iterator it = ECAN.begin(); it != ECAN.end(); it++){
            ID = it->first.as<uint32_t>();
            IDstr = it->first.as<std::string>();
            for(YAML::const_iterator it2 = ECAN[IDstr].begin(); it2 != ECAN[IDstr].end(); it2++){
                CanSignal_t tmp_signal;
                Sigstr = it2->first.as<std::string>();
                tmp_signal.Name = Sigstr;
                tmp_signal.StartBit = ECAN[IDstr][Sigstr]["StartBit"].as<uint16_t>();
                tmp_signal.Length = ECAN[IDstr][Sigstr]["Length"].as<uint16_t>();
                tmp_signal.ByteOrder = ECAN[IDstr][Sigstr]["ByteOrder"].as<std::string>();
                tmp_signal.ValueType = ECAN[IDstr][Sigstr]["ValueType"].as<std::string>();
                tmp_signal.Factor = ECAN[IDstr][Sigstr]["Factor"].as<float>();
                tmp_signal.Offset = ECAN[IDstr][Sigstr]["Offset"].as<uint16_t>();
                if(ECAN[IDstr][Sigstr]["Heart"].IsDefined()){
                    for(int i = 0; i < ECAN[IDstr][Sigstr]["Heart"].size(); i++){
                        tmp_signal.Heart.push_back(ECAN[IDstr][Sigstr]["Heart"][i].as<uint16_t>());
                    }
                }else if(ECAN[IDstr][Sigstr]["CheckByte"].IsDefined()){
                    for(int i = 0; i < ECAN[IDstr][Sigstr]["CheckByte"].size(); i++){
                        tmp_signal.CheckByte.push_back(ECAN[IDstr][Sigstr]["CheckByte"][i].as<uint16_t>());
                    }
                }
                ECanSignalMap[ID].push_back(tmp_signal);
            }
        }
    }
    
    #ifdef CAN_INFO_DEBUG
        for(auto it : this->ECanSignalMap){
            std::cout << "ID:" << it.first << std::endl;
            for(auto ic : it.second){
                std::cout << ic.Name << std::endl;
                std::cout << ic.StartBit << std::endl;
                std::cout << ic.Length << std::endl;
                std::cout << ic.ByteOrder << std::endl;
                std::cout << ic.ValueType << std::endl;
                std::cout << ic.Factor << std::endl;
                std::cout << ic.Offset << std::endl;
            }
        }
    #endif
}

//容错检查函数:检查VEHICLE.yaml文件是否合法
void CanInfo::FaultTolerance(const YAML::Node& node){
    if(!node["DBC"].IsDefined()|| !node["Control"].IsDefined() || !node["FeedBack"].IsDefined()){
        std::cout << ERROR "(_CanInfo): VEHICLE.yaml need some necessary key:" << std::endl;
        std::cout << "1.\tDBC:" << std::endl;
        std::cout << "\t Status" << std::endl;
        std::cout << "\t Name" << std::endl;
        std::cout << "2.\tControl:" << std::endl;
        std::cout << "3.\tFeedBack:" << RESET << std::endl;
        exit(-1);
    }
    if(!node["Action"].IsDefined()){
        std::cout << WARNING "(_CanInfo): If you don't need the Action key, " \
        "you have to fill the vehicle-action signal in the corresponding action " \
        "method of the _VehicleAction class:" << std::endl;
    }
    if(!node["Heart"].IsDefined() || !node["Check"].IsDefined()){
        std::cout << WARNING "(_CanInfo): You have no Heart or Check key, " \
        "please ensure that the car does not require Heartbeat signals or " \
        "Check signals" << std::endl;
    }
}


#endif //CAN_INFO_H