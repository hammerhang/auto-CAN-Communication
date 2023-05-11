/*详情：......*/

#ifndef CAN_AUTO_ANALYSIS_H
#define CAN_AUTO_ANALYSIS_H

#include <iostream>
#include <string>
#include <list>
#include "_CanInfo.h"
#include <linux/can.h>
#include <unordered_map>
#include <math.h>

// typedef std::unordered_map<std::string, float> DataMap_t;

/*CAN报文帧自动解析类*/
class CanAutoAnalysis : virtual public CanInfo
{
    private:
        std::unordered_map<std::string, float> DataMap; //结果数据表
    public:
        void Frame2Data(const can_frame& canframe);
        float GetData(const std::string& signal_name);
    public:
        CanAutoAnalysis();

};


//构造函数：初始化数据表
CanAutoAnalysis::CanAutoAnalysis(){
    this->ACanSignalMapInit(); //获取 ACAN.yaml 信息
    ACanSignalMap_t ACanSignalMap = GetACanSignalMap();
    //初始化DataMap
    for(ACanSignalMap_t::iterator it = ACanSignalMap.begin(); it != ACanSignalMap.end(); ++it){
        for(std::list<CanSignal_t>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            this->DataMap[it2->Name] = 0;
        }
    }
}

float CanAutoAnalysis::GetData(const std::string& signal_name){
    return this->DataMap[signal_name];
}


//将can_frame解析到数据表中
void CanAutoAnalysis::Frame2Data(const can_frame& canframe){
    ACanSignalMap_t ACanSignalMap = GetACanSignalMap();
    std::list<CanSignal_t> ACanSignalList(ACanSignalMap[(uint32_t)canframe.can_id]);
    uint8_t bitmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
    for(std::list<CanSignal_t>::iterator it = ACanSignalList.begin(); it != ACanSignalList.end(); ++it){
        int8_t remain_bit = it->Length;
        int32_t value = 0;
        uint8_t temp = 0;
        uint8_t sbyte = it->StartBit / 8; //起始字节
        uint8_t sbit = it->StartBit % 8; //起始字节中的起始位
        if(it->ByteOrder == "Motorola MSB")
        {
            it->StartBit = ((it->Length - sbit - 1)/8 + sbyte + 1) * 8 + 8 - ((it->Length - sbit - 1) % 8); //将motorola_msb的起始位序号转换为motorola_lsb的起始位序号
            //更新 sbyte 和 sbit
            sbyte = it->StartBit / 8;
            sbit = it->StartBit % 8;
        }
        while (remain_bit > 0)
        {
            uint8_t dwonce = ((remain_bit + sbit) > 8 ? 8 : (remain_bit + sbit)) - sbit; //每次循环处理这么多位数据
            value |= ((int32_t)(canframe.data[sbyte] & (uint8_t)((bitmask[dwonce] << sbit))) >> sbit) << (it->Length - remain_bit);
            sbyte = it->ByteOrder == "Intel" ? ++sbyte : --sbyte;
            sbit = 0; //起始位清零
            remain_bit -= dwonce;
        }
        if(it->ValueType == "Signed" && value > pow(2 , it->Length - 1)) value -= pow(2 , it->Length); //符号数超过 2^(Length-1) 时为负数
        this->DataMap[it->Name] = value * it->Factor + it->Offset;
    }
}

#endif //CAN_AUTO_ANALYSIS_H