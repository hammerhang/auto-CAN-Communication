#ifndef MSG_H
#define MSG_H

#include <iostream>
#include <string>
#include <vector>

#define RESET       "\033[0m"
#define RED         "\033[31m"          /* Red */           //错误输出颜色
#define GREEN       "\033[32m"          /* Green */         //调试输出颜色
#define YELLOW      "\033[33m"          /* Yellow */        //警告输出颜色
#define WHITE       "\033[37m"          /* White */         //普通信息输出颜色

#define INFO        WHITE"[INFO] "
#define DEBUG       GREEN"[DEBUG] "
#define WARNING     YELLOW"[WARNING] "
#define ERROR       RED"[ERROR] "

typedef std::unordered_map<std::string, float> CmdMap_t;

typedef struct CanSignal{
    std::string Name;
    uint16_t StartBit;
    uint16_t Length;
    std::string ByteOrder;
    std::string ValueType;
    float Factor;
    float Offset;
    double Max;
    double Min;
    std::string Unit;
    std::string Receiver;
    std::vector<uint16_t> Heart;
    std::vector<uint8_t> CheckByte;
} CanSignal_t;


#endif