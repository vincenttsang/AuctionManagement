//
//  utilities.cpp
//  物品竞拍管理
//
//  Created by Vincent Tsang on 15/4/2021.
//

#include <uuid/uuid.h> //在macOS或Linux平台上用于生成UUID
#include <sys/stat.h>
#include <string>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <array>
using std::string;

bool isFileExist(const string& name);
void GenerateUUID(string &id);
void clear(void);
string GetLocalTime(void);

void GenerateUUID(string &id){
    char* id_string_in_c_style = new char[200]();
    uuid_t uuid;
    uuid_generate(uuid);
    for(int i = 0; i < 16; i++){
        sprintf(id_string_in_c_style + i*2, "%02X", uuid[i]); //16进制数转字符
    }
    id = id_string_in_c_style;
} //生成UUID并转换为字符串

bool isFileExist(const std::string& name){
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
} //使用POSIX stat方式检测文件存在

void clear(void){
    // CSI[2J clears screen, CSI[H moves the cursor to top-left corner
    std::cout << "\x1B[2J\x1B[H";
}

string GetLocalTime(void){
    std::array<char, 64> buffer;
    buffer.fill(0);
    time_t rawtime;
    time(&rawtime);
    const auto timeinfo = localtime(&rawtime);
    strftime(buffer.data(), sizeof(buffer), "[%Y/%m/%d %H:%M:%S] ", timeinfo);
    std::string timeStr(buffer.data());
    return timeStr;
}
