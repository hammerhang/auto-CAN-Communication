#ifndef VEHICLE_ACTION_H
#define VEHICLE_ACTION_H

#include <iostream>
#include <yaml-cpp/yaml.h>
#include "_Msg.h"

//车辆动作类
class VehicleAction
{
    private:
        char* config_dir_path_env = std::getenv("config_path_dir");
        std::string config_path = config_dir_path_env;
        YAML::Node CONFIG = YAML::LoadFile(config_path + "/controlConfig.yaml");
        YAML::Node VEHICLE = YAML::LoadFile(config_path + "/VehicleCanInfo/" + CONFIG["Vehicle"].as<std::string>() + "/VEHICLE.yaml");
    public:
        void Advance(CmdMap_t& cmd, float speed);   //前进动作
        void Back(CmdMap_t& cmd, float speed);      //后退动作
        void Left(CmdMap_t& cmd, float angle);      //左转动作
        void Right(CmdMap_t& cmd, float angle);     //右转动作
        void Stop(CmdMap_t& cmd);                   //停止动作
        void AEB(CmdMap_t& cmd);                    //急刹动作
};

void VehicleAction::Advance(CmdMap_t& cmd, float speed)
{
    if(VEHICLE["Action"].IsDefined()){
        if(VEHICLE["Action"]["Advance"].IsDefined()){
            for(int i = 0; i < VEHICLE["Action"]["Advance"].size(); i++){
                YAML::Node::const_iterator it = VEHICLE["Action"]["Advance"][i].begin();
                if(it->second.as<std::string>() == "variate")
                    cmd[it->first.as<std::string>()] = speed;
                else
                    cmd[it->first.as<std::string>()] = it->second.as<float>();
            }
        }else{
            std::cout << WARNING "(_VehicleAction): no key is defined named \"Advance\"" RESET << std::endl;
        }
    }else{
        std::cout << WARNING "(_VehicleAction): no key is defined named \"Action\"" RESET << std::endl;
        /*自定义区域*/
    }
    // cmd["ACU_ChassisBrakeEn"] = 0;
    // cmd["ACU_ChassisDriverEnCtrl"] = 1;
    // cmd["ACU_ChassisDriverModeCtrl"] = 0;
    // cmd["ACU_ChassisGearCtrl"] = 1;
    // cmd["ACU_ChassisSpeedCtrl"] = speed;  
}

void VehicleAction::Back(CmdMap_t& cmd, float speed)
{
    if(VEHICLE["Action"].IsDefined()){
        if(VEHICLE["Action"]["Back"].IsDefined()){
            for(int i = 0; i < VEHICLE["Action"]["Back"].size(); i++){
                YAML::Node::const_iterator it = VEHICLE["Action"]["Back"][i].begin();
                if(it->second.as<std::string>() == "variate")
                    cmd[it->first.as<std::string>()] = speed;
                else
                    cmd[it->first.as<std::string>()] = it->second.as<float>();
            }
        }else{
            std::cout << WARNING "(_VehicleAction): no key is defined named \"Back\"" RESET << std::endl;
        }
    }else{
        std::cout << WARNING "(_VehicleAction): no key is defined named \"Action\"" RESET << std::endl;
    }
    // cmd["ACU_ChassisBrakeEn"] = 0;
    // cmd["ACU_ChassisDriverEnCtrl"] = 1;
    // cmd["ACU_ChassisDriverModeCtrl"] = 0;
    // cmd["ACU_ChassisGearCtrl"] = 3;
    // cmd["ACU_ChassisSpeedCtrl"] = fabs(speed);
}

void VehicleAction::Left(CmdMap_t& cmd, float angle)
{
    if(VEHICLE["Action"].IsDefined()){
        if(VEHICLE["Action"]["Left"].IsDefined()){
            for(int i = 0; i < VEHICLE["Action"]["Left"].size(); i++){
                YAML::Node::const_iterator it = VEHICLE["Action"]["Left"][i].begin();
                if(it->second.as<std::string>() == "variate")
                    cmd[it->first.as<std::string>()] = angle * 10;
                else
                    cmd[it->first.as<std::string>()] = it->second.as<float>();
            }
        }else{
            std::cout << WARNING "(_VehicleAction): no key is defined named \"Left\"" RESET << std::endl;
        }
    }else{
        std::cout << WARNING "(_VehicleAction): no key is defined named \"Action\"" RESET << std::endl;
    }
    // cmd["ACU_ChassisSteerEnCtrl"] = 1;
    // cmd["ACU_ChassisSteerModeCtrl"] = 0;
    // cmd["ACU_ChassisSteerAngleTarget"] = angle * 10;
}

void VehicleAction::Right(CmdMap_t& cmd, float angle)
{
    if(VEHICLE["Action"].IsDefined()){
        if(VEHICLE["Action"]["Right"].IsDefined()){
            for(int i = 0; i < VEHICLE["Action"]["Right"].size(); i++){
                YAML::Node::const_iterator it = VEHICLE["Action"]["Right"][i].begin();
                if(it->second.as<std::string>() == "variate")
                    cmd[it->first.as<std::string>()] = angle * 10;
                else
                    cmd[it->first.as<std::string>()] = it->second.as<float>();
            }
        }else{
            std::cout << WARNING "(_VehicleAction): no key is defined named \"Right\"" RESET << std::endl;
        }
    }else{
        std::cout << WARNING "(_VehicleAction): no key is defined named \"Action\"" RESET << std::endl;
    }
    // cmd["ACU_ChassisSteerEnCtrl"] = 1;
    // cmd["ACU_ChassisSteerModeCtrl"] = 0;
    // cmd["ACU_ChassisSteerAngleTarget"] = angle * 10;
}

void VehicleAction::Stop(CmdMap_t& cmd)
{
    if(VEHICLE["Action"].IsDefined()){
        if(VEHICLE["Action"]["Stop"].IsDefined()){
            for(int i = 0; i < VEHICLE["Action"]["Stop"].size(); i++){
                YAML::Node::const_iterator it = VEHICLE["Action"]["Stop"][i].begin();
                // if(it->second.as<std::string>() == "variate")
                //     cmd[it->first.as<std::string>()] = angle;
                // else
                    cmd[it->first.as<std::string>()] = it->second.as<float>();
            }
        }else{
            std::cout << WARNING "(_VehicleAction): no key is defined named \"Stop\"" RESET << std::endl;
        }
    }else{
        std::cout << WARNING "(_VehicleAction): no key is defined named \"Action\"" RESET << std::endl;
    }
    // cmd["ACU_ChassisBrakeEn"] = 1;
    // cmd["ACU_ChassisBrakePdlTarget"] = 50;
}

void VehicleAction::AEB(CmdMap_t& cmd)
{
    if (VEHICLE["Action"].IsDefined())
    {
        if (VEHICLE["Action"]["AEB"].IsDefined())
        {
            for (int i = 0; i < VEHICLE["Action"]["AEB"].size(); i++)
            {
                YAML::Node::const_iterator it = VEHICLE["Action"]["AEB"][i].begin();
                cmd[it->first.as<std::string>()] = it->second.as<float>();
            }
        }
        else
        {
            std::cout << WARNING "(_VehicleAction): no key is defined named \"AEB\"" RESET << std::endl;
        }
    }
    else
    {
        std::cout << WARNING "(_VehicleAction): no key is defined named \"Action\"" RESET << std::endl;
    }
}

#endif //VEHICLE_ACTION_H
