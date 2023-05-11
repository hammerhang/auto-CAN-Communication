/*详情：......*/

#ifndef CAN_AUTO_ENCAPSULATION_H
#define CAN_AUTO_ENCAPSULATION_H

#include <iostream>
#include <string>
#include <list>
#include "_CanInfo.h"
#include <linux/can.h>
#include <unordered_map>

// #define CAN_AUTO_ENCAPSULATION_DEBUG

// typedef std::unordered_map<uint32_t, can_frame> CanFrameMap_t; //can_frame表

/*CAN报文帧自动封装类*/
class CanAutoEncapsulation : virtual public CanInfo
{
    public:
        CmdMap_t CmdMap; //指令表
    private:
        std::unordered_map<uint32_t, can_frame> FrameMap; //can_frame表
    public:
        CanAutoEncapsulation();
        // void CmdUpData();
        void Data2Frame();
        std::unordered_map<uint32_t, can_frame> GetFrameMap();
};

//构造函数：初始化指令表和can_frame表 
CanAutoEncapsulation::CanAutoEncapsulation(){
    this->CanInfo::ECanSignalMapInit(); //获取 ECAN.yaml 信息，存入 CanInfo::ECanSignalMap
    ECanSignalMap_t ECanSignalMap = CanInfo::GetECanSignalMap();
    //初始化指令表和can_frame表
    for(ECanSignalMap_t::iterator it = ECanSignalMap.begin(); it != ECanSignalMap.end(); ++it){
        for(std::list<CanSignal_t>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            if(!it2->Heart.empty())
                this->CmdMap[it2->Name+std::to_string(it->first)] = it2->Heart[0];
            else if(!it2->CheckByte.empty())
                this->CmdMap[it2->Name+std::to_string(it->first)] = 0; //添加can_id后缀：为了防止心跳信号与校验信号名字重复导致键值对重合的情况
            else
                this->CmdMap[it2->Name] = 0;
        }
        this->FrameMap[it->first].can_id = it->first;
        this->FrameMap[it->first].can_dlc = 8;
    }
}

std::unordered_map<uint32_t, can_frame> CanAutoEncapsulation::GetFrameMap(){
    return this->FrameMap;
}

// //指令表更新
// void CanAutoEncapsulation::CmdUpData(){

// }

//将指令表数据填充进can_frame表
void CanAutoEncapsulation::Data2Frame(){
    uint8_t bitmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
    ECanSignalMap_t ECanSignalMap = CanInfo::GetECanSignalMap();
    for(ECanSignalMap_t::iterator it = ECanSignalMap.begin(); it != ECanSignalMap.end(); ++it){
        for(std::list<CanSignal_t>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            int8_t remain_bit = it2->Length;
            uint8_t temp = 0;
            uint8_t sbyte = it2->StartBit / 8; //起始字节
            uint8_t sbit = it2->StartBit % 8; //起始字节中的起始位
            int32_t value = (this->CmdMap[it2->Name] - it2->Offset) / it2->Factor;
            //查看是否是心跳信号
            HeartMap_t HeartSignalMap = CanInfo::GetHeartSignalMap();
            if(HeartSignalMap.find(it2->Name) != HeartSignalMap.end()){
                if((uint16_t)this->CmdMap[it2->Name+std::to_string(it->first)] > HeartSignalMap[it2->Name].max){
                    this->CmdMap[it2->Name+std::to_string(it->first)] = HeartSignalMap[it2->Name].min;
                }
                value = (this->CmdMap[it2->Name+std::to_string(it->first)] - it2->Offset) / it2->Factor;
                this->CmdMap[it2->Name+std::to_string(it->first)]++;
            }
            //查看是否是校验信号
            CheckMap_t CheckSignalMap = CanInfo::GetCheckSignalMap();
            if(CheckSignalMap.find(it2->Name) != CheckSignalMap.end()){
                int32_t check = 0;
                    for(auto cb : CheckSignalMap[it2->Name].byte){
                        if(CheckSignalMap[it2->Name].mode == "xor" /*异或校验*/) check ^= FrameMap[it->first].data[cb];
                    }
                this->CmdMap[it2->Name+std::to_string(it->first)] = check;
                value = (this->CmdMap[it2->Name+std::to_string(it->first)] - it2->Offset) / it2->Factor;
            }
            //检查字节序
            if(it2->ByteOrder == "Motorola MSB")
            {
                it2->StartBit = ((it2->Length - sbit - 1)/8 + sbyte + 1) * 8 + 8 - ((it2->Length - sbit - 1) % 8); //将motorola_msb的起始位序号转换为motorola_lsb的起始位序号
                //更新 sbyte 和 sbit
                sbyte = it2->StartBit / 8;
                sbit = it2->StartBit % 8;
            }
            //开始封装
            while (remain_bit > 0)
            {
                uint8_t dwonce = ((remain_bit + sbit) > 8 ? 8 : (remain_bit + sbit)) - sbit; //每次循环处理这么多位数据
                temp = static_cast<uint8_t>((value >> (it2->Length - remain_bit))); //得到信号量的相应位
                this->FrameMap[it->first].data[sbyte] &= (uint8_t)(~(bitmask[dwonce] << sbit)); //相应位清零
                this->FrameMap[it->first].data[sbyte] |= ((temp&bitmask[dwonce]) << sbit); //填充相应位
                sbyte = it2->ByteOrder == "Intel" ? ++sbyte : --sbyte;
                sbit = 0; //起始位清零
                remain_bit -= dwonce;
            }
        }
        #ifdef CAN_AUTO_ENCAPSULATION_DEBUG
            for(int i = 0; i < 9; i++){
                if(i == 0){
                    printf("ID:0X%02X\t\t", this->FrameMap[it->first].can_id);
                }else{
                    printf("%02X\t", this->FrameMap[it->first].data[i-1]);
                }
            } 
            std::cout << std::endl;
        #endif
    }
}

#endif //CAN_AUTO_ENCAPSULATION_H