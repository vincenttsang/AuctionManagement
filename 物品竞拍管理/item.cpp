//
//  item.cpp
//  物品竞拍管理
//
//  Created by Vincent Tsang on 15/4/2021.
//

#include "item.hpp"
#include "utilities.hpp"

Item::Item(void){
    item_name = " ";
    item_owner = " ";
    item_uuid = " ";
    item_condition_in_number = 0;
    item_condition = " ";
    item_introduction = " ";
    item_price = 0;
    item_auction_started = false;
} //构造函数

Item::~Item(void){
} //析构函数

void Item::set_item_name(std::string name_from_input){
    this->item_name = name_from_input;
}

void Item::set_item_owner(std::string owner_from_input){
    this->item_owner = owner_from_input;
}

void Item::set_item_uuid(std::string uuid_from_input){
    this->item_uuid = uuid_from_input;
}

void Item::set_item_condition_in_number(item_condition_num num_from_input){
    this->item_condition_in_number = num_from_input;
}

void Item::set_item_condition(std::string condition_from_input){
    this->item_condition = condition_from_input;
}

void Item::set_item_introduction(std::string introduction_from_input){
    this->item_introduction = introduction_from_input;
}

std::string Item::show_item_uuid(void){
    return this->item_uuid;
}

std::string Item::show_item_name(void){
    return this->item_name;
}

std::string Item::show_item_owner(void){
    return this->item_owner;
}

std::string Item::show_item_info(void){
    return this->item_introduction;
}

std::string Item::show_item_condition(void){
    return this->item_condition;
}

item_condition_num Item::show_item_condition_in_number(void){
    return this->item_condition_in_number;
}

unsigned long Item::show_item_price(void){
    return this->item_price;
}

void Item::set_item_price(unsigned long price){
    this->item_price = price;
}

void Item::start_auction(void){
    this->item_auction_started = true;
}

void Item::stop_auction(void){
    this->item_auction_started = false;
}

bool Item::is_auction_started(void){
    return this->item_auction_started;
}

void Item::SaveToDisk(std::string filename){
    json obj;
    std::ofstream obj_file;
    filename += ".item.json";
    obj["item_name"] = this->item_name;
    obj["item_owner"] = this->item_owner;
    obj["item_uuid"] = this->item_uuid;
    obj["item_condition_in_number"] = this->item_condition_in_number;
    obj["item_condition"] = this->item_condition;
    obj["item_introduction"] = this->item_introduction;
    obj["item_price"] = this->item_price;
    obj["item_auction_started"] = this->item_auction_started;
    obj_file.open(filename);
    obj_file << std::setw(4) << obj << std::endl;
    obj_file.close();
}

void Item::ReadFromDisk(std::string filename){
    if(isFileExist(filename)){
        std::ifstream obj_file(filename);
        json obj = json::parse(obj_file);
        this->item_name = obj["item_name"];
        this->item_owner = obj["item_owner"];
        this->item_uuid = obj["item_uuid"];
        this->item_condition_in_number = obj["item_condition_in_number"];
        this->item_condition = obj["item_condition"];
        this->item_introduction = obj["item_introduction"];
        this->item_price = obj["item_price"];
        this->item_auction_started = obj["item_auction_started"];
        obj_file.close();
    }
}
