/* 详情... */

#ifndef DBC_ANALYSIS_H
#define DBC_ANALYSIS_H

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp> //正则表达式
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "_Msg.h"
#include <list>

typedef struct CanMessage{
    uint32_t ID; //报文ID
    std::string Name; //报文名字
    uint16_t dlc; //数据长度代码 DLC（DateLengthCode）
    std::string Transmitter; //发送节点
    std::list<CanSignal_t> SignalList; //信号列表
} CanMessage_t;

typedef std::unordered_map<uint32_t, CanMessage_t> CanMessageMap_t;


//dbc解析类
class DBCAnalysis
{
    private:
        std::vector<std::string> BO_Analysis(const std::string& BO_line);
        std::vector<std::string> SG_Analysis(std::string& SG_line);
    protected:
        CanMessageMap_t CanMessageMap;
        void getDBC(const std::string& filepath);
};


//解析DBC文件
void DBCAnalysis::getDBC(const std::string& filepath){
    std::ifstream inFile(filepath);
    std::string line;
    while(std::getline(inFile, line)){
        CanMessage_t canmessage_temp;
        bool SGflag = false;
        if(line[line.length() - 1] == '\r') line.erase(line.end()-1); //兼容windows和linux的换行符 "\r\n" 和 "\n"
        if(line.length() == 0) continue;
        boost::erase_all(line, " "); //删除空格
        boost::erase_all(line, "\t"); //删除制表符
        //"BO_" 报文信息获取
        if(boost::starts_with(line, "BO_")){
            if(line.length() <= 3 || !isdigit(line[3])) continue; //真正的报文信息行，BO_ 后面应该紧接着报文ID
            std::vector<std::string> MessageInfo = BO_Analysis(line); //解析BO_行
            canmessage_temp.ID = stoi(MessageInfo[0]);
            canmessage_temp.Name = MessageInfo[1];
            canmessage_temp.dlc = stoi(MessageInfo[2]);
            canmessage_temp.Transmitter = MessageInfo[3];
            CanMessageMap[canmessage_temp.ID] = canmessage_temp;
            SGflag = true; //后续SG_才是信号
        }
        //"SG_" 报文信息获取
        if(boost::starts_with(line, "SG_")){
            boost::regex expr{"SG_(\\w+)[:](\\d+)[|](\\d+)[@](\\d+)([+]|[-])[(](.*)[,](.*)[)][[](.*)[|](.*)[]][\"](.*)[\"](\\w+)"};
            boost::smatch what;
            if(boost::regex_search(line, what, expr)){
                CanSignal_t cansignal_temp;
                cansignal_temp.Name = what[1];
                cansignal_temp.StartBit = stoi(what[2]);
                cansignal_temp.Length = stoi(what[3]);
                cansignal_temp.ByteOrder = what[4] == "0" ? "Motorola" : "Intel";
                cansignal_temp.ValueType = what[5] == "+" ? "Unsigned" : "Signed";
                cansignal_temp.Factor = stof(what[6]);
                cansignal_temp.Offset = stof(what[7]);
                cansignal_temp.Min = stod(what[8]);
                cansignal_temp.Max = stod(what[9]);
                cansignal_temp.Unit = what[10];
                cansignal_temp.Receiver = what[11];
                CanMessageMap[canmessage_temp.ID].SignalList.push_back(cansignal_temp);
            }
        }
    }
}

// //处理"SG_"行
// std::vector<std::string> SG_Analysis(std::string& SG_line){
//     return;
// }

//处理"BO_"行（暂时没想到很好的算法）
// std::vector<std::string> DBCAnalysis::BO_Analysis(std::string& BO_line){
//     uint8_t count = 0;
//     std::vector<std::string> res;
//     std::string tmp;
//     boost::erase_first(BO_line, "BO_"); //删除"BO_"
//     for(std::string::iterator it = BO_line.begin(); it != BO_line.end(); ++it){
//         switch(count){
//             case 0 : {
//                 if(isdigit(*it)){
//                     tmp += *it;
//                 }
//                 else{
//                     --it;
//                     res.push_back(tmp);
//                     tmp.clear();
//                     ++count;
//                 }
//                 break;
//             }
//             case 1 :{
//                 if(*it != ':'){
//                     tmp += *it;
//                 }else{
//                     res.push_back(tmp);
//                     tmp.clear();
//                     ++count;
//                 }
//                 break;
//             }
//             case 2 : {
//                 if(isdigit(*it)){
//                     tmp += *it;
//                 }
//                 else{
//                     --it;
//                     res.push_back(tmp);
//                     tmp.clear();
//                     ++count;
//                 }
//                 break;
//             }
//             case 3 : {
//                 tmp += *it;
//                 if(it == BO_line.end() - 1){
//                     res.push_back(tmp);
//                 }
//                 break;
//             }
//         }
//     }
//     return res;
// }

std::vector<std::string> DBCAnalysis::BO_Analysis(const std::string& BO_line) {
	uint8_t index = 3;
	std::string temp;
	std::vector<std::string> res;
    // boost::erase_first(BO_line, "BO_"); //删除"BO_"
	for (int i = 0; i < 4; i++) {
		while (i == 0 && isdigit(BO_line[index]))	temp += BO_line[index++];
		while (i == 1 && BO_line[index] != ':')		temp += BO_line[index++];
		while (i == 2 && isdigit(BO_line[index]))	temp += BO_line[index++];
		while (i == 3 && index <BO_line.size())		temp += BO_line[index++];
		res.push_back(temp);
		temp.clear();
		if (i == 1) index++;
	}
	return res;
}

#endif