//
//  communication.cpp
//  物品竞拍管理
//
//  Created by Vincent Tsang on 15/4/2021.
//

#include "communication.hpp"
#include <iostream>
#include <string>

extern UserList* default_userlist;

void tcp_connection::ProcessRequest(std::string str){
    int opcode = 0;
    json request;
    try {
        request = json::parse(str);
        opcode = request["opcode"];
    }
    catch (json::parse_error& ex) {
        std::cerr << "parse error at byte " << ex.byte << std::endl;
    }
    if(opcode == 0){
        try {
            std::string username = request["username"];
            std::string password = request["token"];
            if(UserLogin(username, password, default_userlist)){
                std::cout << GetLocalTime() << "用户 [" << username << "] 登录成功" << std::endl;
                data_in_string = "服务器: 登录成功"; // 将要在do_write()回应客户端的内容
            }
            else{
                data_in_string = "服务器: FUCK";
                std::cout << GetLocalTime() << "用户 [" << username << "] 登录失败" << std::endl;
            }
        }
        catch (json::parse_error& ex) {
            std::cerr << "parse error at byte " << ex.byte << std::endl;
        }
    }
    
    if(opcode == 1){
        try {
            std::string username = request["username"];
            std::string token = request["token"];
            std::cout << GetLocalTime() << "来自客户端的用户名 [" << username << "]" <<std::endl;
            std::cout << GetLocalTime() << "来自客户端的用户密码 [" << token << "]" << std::endl;
            std::cout << GetLocalTime() << "来自客户端的物品名 [" << request["name"] << "]" << std::endl;
            std::cout << GetLocalTime() << "来自客户端的物品介绍 [" << request["info"] << "]" << std::endl;
            std::cout << GetLocalTime() << "来自客户端的物品新旧程度描述 [" << request["condition"] << "]" << std::endl;
            std::cout << GetLocalTime() << "来自客户端的物品新旧程度数值 [" << request["condition_in_num"] << "]" << std::endl;
            std::cout << GetLocalTime() << "客户端请求的操作模式为 [录入物品信息]" << std::endl;
            if(UserLogin(username, token, default_userlist)){
                data_in_string = "服务器: 登录成功";
                std::cout << GetLocalTime() << "用户 [" << username << "] 登录成功" << std::endl;
                Item new_item;
                std::string new_uuid;
                GenerateUUID(new_uuid);
                std::cout << GetLocalTime() << "给予物品新UUID：" << new_uuid << std::endl;
                new_item.set_item_name(request["name"]);
                new_item.set_item_condition(request["condition"]);
                new_item.set_item_introduction(request["info"]);
                new_item.set_item_condition_in_number(request["condition_in_num"]);
                new_item.set_item_uuid(new_uuid);
            }
            else{
                data_in_string = "服务器: FUCK";
                std::cout << GetLocalTime() << "用户 [" << username << "] 登录失败" << std::endl;
            }
        }
        
        catch (std::exception& e) {
          std::cerr << e.what() << std::endl;
        }
    }
    
}

void tcp_connection::do_read(void){
    buffer_ = new char[BUFFER_SIZE]();
    socket_.async_read_some(boost::asio::buffer(buffer_, BUFFER_SIZE),
                            boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
    data_in_string.assign(buffer_);
    delete [] buffer_;
    //std::cout << GetLocalTime() << "从客户端收到请求：" << data_in_string << std::endl;
    std::cout << GetLocalTime() << "收到客户端请求" << std::endl;
    
}

void tcp_connection::do_write(std::string str){
    message_ = str; //回应客户端
    
    boost::asio::async_write(socket_, boost::asio::buffer(message_),
                             boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
    std::cout << GetLocalTime() << "已发送：" << this->message_ << std::endl;
}

int RunServer(){
    try {
      boost::asio::io_context io_context;
      tcp_server server(io_context);
      io_context.run();
    }
    catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
    return 0;
}
