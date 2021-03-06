//
//  client.cpp
//  物品竞拍管理客户端
//
//  Created by Vincent Tsang on 15/4/2021.
//

#include "client.hpp"

unsigned long seconds = 0;

client_tcp_connection::client_tcp_connection(){
    _socket = new tcp::socket(_io);
    _endpoint = new tcp::endpoint(boost::asio::ip::address::from_string(ip_address), 11451);
}

void client_tcp_connection::client_send(json j){
    try {
        std::string data = j.dump();
        this->_socket->connect(*(this->_endpoint));
        
        boost::system::error_code ec;
        this->_socket->write_some(boost::asio::buffer(data), ec); // 发送数据
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
    }
}

void client_tcp_connection::client_recv(){
    try {
        boost::array<char, 81920> buf;
        boost::system::error_code error;
        size_t len = this->_socket->read_some(boost::asio::buffer(buf), error);
        
        this->data_from_server.assign(buf.data(), len);
        // cout << this->data_from_server << endl;
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
    }
}

void client_tcp_connection::client_process_data(bool& result){
    int response_code;
    try {
        this->response_from_server = json::parse(this->data_from_server);
        response_code = this->response_from_server["response_code"];
        if(response_code == 1){
            result = true;
        }
        if(response_code == 0){
            result = false;
        }
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
    }
}

json client_tcp_connection::show_recv_json(){
    try {
        this->response_from_server = json::parse(this->data_from_server);
        return response_from_server;
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
        json j;
        return j;
    }
}

int UserMenu(void){
    int op = -1;
    cout << "欢迎使用物品竞拍管理系统" << endl << "请先登录" << endl;
    while(op != 0){
        cout << "1.用户登录\n2.用户注册\n3.退出程序" << endl;
        cin.clear();
        fflush(stdin);
        scanf("%d", &op);
        cin.clear();
        fflush(stdin);
        switch (op) {
            case 1:
                cout << "请输入用户名：";
                getline(cin,username);
                cout << "请输入用户密码：";
                getline(cin,password);
                if(ClientUserLogin(username, password)){
                    cout << "用户登录成功" << endl;
                    return SUCCESS;
                }
                else{
                    cout << "登录失败 请检查你的用户名或密码" << endl;
                    return ERROR;
                }
                break;
            case 2:
                cout << "请输入用户名：";
                getline(cin,username);
                cout << "请输入用户密码：";
                getline(cin,password);
                if(ClientRegisterUser(username, password)){
                    cout << "用户注册成功 请登录注册的账户" << endl;
                    return ERROR; // 返回登录菜单
                }
                else{
                    cout << "注册失败 用户名已存在" << endl;
                    return ERROR;
                }
                break;
            case 3:
                cout << "感谢使用" << endl;
                exit(0);
                break;
            default:
                cin.clear();
                fflush(stdin);
                return ERROR;
                break;
        }
    }
    return ERROR;
}

int main(int argc, const char * argv[]){
    for(;UserMenu()!=SUCCESS);
    int op = -1;
    while(op != 0){
        cout << "欢迎使用物品竞拍管理系统" << endl << "请从菜单中选择功能：" << endl;
        cout << "1.录入物品信息\n2.修改物品信息\n3.删除物品\n4.显示物品信息\n5.使用管理员账户开始拍卖\n6.普通用户进入拍卖\n7.管理员列出所有物品" << endl;
        cout << "0.退出程序" << endl;
        cin.clear();
        fflush(stdin);
        scanf("%d", &op);
        switch (op) {
            case 0:
                cout << "感谢使用" << endl;
                return 0;
                break;
            case 1:
                if(RecordInformation()){
                    cout << "录入完成\n\n";
                }
                else{
                    cout << "录入失败\n\n";
                }
                break;
            case 2:
                if(EditInformation()){
                    cout << "修改完成\n\n";
                }
                else{
                    cout << "你要修改的物品不存在或不属于你 请核对你输入的UUID\n\n";
                }
                break;
            case 3:
                if(DeleteInformation()){
                    cout << "删除完成\n\n";
                }
                else{
                    cout << "你要删除的物品不存在或不属于你 请核对你输入的UUID\n\n";
                }
                break;
            case 4:
                if(!ShowItemInformation()){
                    cout << "你要查找的物品不存在或不属于你 请核对你输入的UUID\n\n";
                }
                break;
            case 5:
                if(!StartAuction()){
                    cout << "拍卖失败\n\n";
                }
                break;
            case 6:
                BidMenu();
                break;
            case 7:
                if(!AdminLoadItems()){
                    
                }
            default:
                clear();
                cin.clear();
                fflush(stdin);
                break;
        }
    }
    return 0;
}

bool ClientUserLogin(std::string username, std::string password){
    json login;
    bool result;
    login["opcode"] = 0; // 0为登录opcode
    login["username"] = username;
    login["token"] = password;
    client_tcp_connection* new_connection = new client_tcp_connection();
    new_connection->client_send(login);
    new_connection->client_recv();
    new_connection->client_process_data(result);
    delete new_connection;
    return result;
}

bool ClientRegisterUser(std::string username, std::string password){
    json reg;
    bool result;
    reg["opcode"] = 114514; // 0为登录opcode
    reg["username"] = username;
    reg["token"] = password;
    client_tcp_connection* new_connection = new client_tcp_connection();
    new_connection->client_send(reg);
    new_connection->client_recv();
    new_connection->client_process_data(result);
    delete new_connection;
    return result;
}

bool RecordInformation(void){
    json new_item;
    std::string name,condition,info;
    unsigned int condition_num = 0;
    bool result;
    cout << "请输入物品信息" << endl;
    cin.clear();
    fflush(stdin);
    cout << "请输入物品名：";
    getline(cin,name);
    cout << "请输入物品描述：";
    getline(cin,info);
    cout << "请输入物品新旧程度描述：";
    getline(cin,condition);
    cout << "请输入物品新旧程度数（范围0-10，10为十成新，0为战损版，以此类推）：";
    cin >> condition_num;
    new_item["opcode"] = 1;
    new_item["username"] = username;
    new_item["token"] = password;
    new_item["name"] = name;
    new_item["info"] = info;
    new_item["condition_in_num"] = condition_num;
    new_item["condition"] = condition;
    client_tcp_connection* new_connection = new client_tcp_connection();
    new_connection->client_send(new_item);
    new_connection->client_recv();
    new_connection->client_process_data(result);
    delete new_connection;
    return result;
}

bool EditInformation(void){
    json edit_item;
    std::string uuid,name,condition,info;
    unsigned int condition_num = 0;
    bool result;
    cout << "请输入一个需要修改物品的UUID" << endl;
    cin.clear();
    fflush(stdin);
    cin >> uuid;
    cout << "请重新输入物品信息" << endl;
    cin.clear();
    fflush(stdin);
    cout << "请输入物品名：";
    getline(cin,name);
    cout << "请输入物品描述：";
    getline(cin,info);
    cout << "请输入物品新旧程度描述：";
    getline(cin,condition);
    cout << "请输入物品新旧程度数（范围0-10，10为十成新，0为战损版，以此类推）：";
    cin >> condition_num;
    edit_item["opcode"] = 2;
    edit_item["username"] = username;
    edit_item["token"] = password;
    edit_item["uuid"] = uuid;
    edit_item["name"] = name;
    edit_item["info"] = info;
    edit_item["condition_in_num"] = condition_num;
    edit_item["condition"] = condition;
    client_tcp_connection* new_connection = new client_tcp_connection();
    new_connection->client_send(edit_item);
    new_connection->client_recv();
    new_connection->client_process_data(result);
    delete new_connection;
    return result;
}

bool DeleteInformation(void){
    json edit_item;
    std::string uuid;
    bool result;
    cout << "请输入一个需要删除物品的UUID" << endl;
    cin.clear();
    fflush(stdin);
    cin >> uuid;
    edit_item["opcode"] = 3;
    edit_item["username"] = username;
    edit_item["token"] = password;
    edit_item["uuid"] = uuid;
    client_tcp_connection* new_connection = new client_tcp_connection();
    new_connection->client_send(edit_item);
    new_connection->client_recv();
    new_connection->client_process_data(result);
    
    if(result){
        json recv = new_connection->show_recv_json();
        std::string name = recv["name"];
        cout << "已删除名为 [" << name <<"] 的物品";
    }
    
    delete new_connection;
    return result;
}

bool ShowItemInformation(void){
    json show_info;
    std::string uuid;
    bool result;
    cout << "请输入你要查找物品的UUID" << endl;
    cin.clear();
    fflush(stdin);
    cin >> uuid;
    show_info["opcode"] = 4;
    show_info["username"] = username;
    show_info["token"] = password;
    show_info["uuid"] = uuid;
    client_tcp_connection* new_connection = new client_tcp_connection();
    new_connection->client_send(show_info);
    new_connection->client_recv();
    new_connection->client_process_data(result);
    
    if(result){
        json recv = new_connection->show_recv_json();
        std::string name = recv["name"];
        std::string owner = recv["owner"];
        std::string info = recv["info"];
        std::string condition = recv["condition"];
        unsigned int condition_in_num = recv["condititon_in_num"];
        cout << "物品名：" << name << endl;
        cout << "物品所有者：" << owner << endl;
        cout << "物品介绍：" << info << endl;
        cout << "物品新旧情况：(" << condition_in_num << ") 详细描述：" << condition << endl;
    }
    
    delete new_connection;
    return result;
}

bool StartAuction(void){
    json auction;
    std::string uuid;
    bool result;
    cout << "请输入要拍卖物品的UUID" << endl;
    fflush(stdin);
    cin >> uuid;
    cout << "请输入拍卖时长（单位：秒）：" << endl;
    fflush(stdin);
    cin >> seconds;
    auction["opcode"] = 5;
    auction["username"] = username;
    auction["token"] = password;
    auction["uuid"] = uuid;
    auction["seconds"] = seconds;
    client_tcp_connection* new_connection = new client_tcp_connection();
    new_connection->client_send(auction);
    new_connection->client_recv();
    new_connection->client_process_data(result);
    
    if(result){
        cout << "已经开始拍卖" << endl;
    }
    
    return result;
}

int BidMenu(void){
    json bid;
    std::string uuid;
    unsigned long price;
    bool result;
    
    cout << "————————出价菜单————————" << endl;
    cout << "请输入目标物品的UUID：" << endl;
    cin.clear();
    fflush(stdin);
    cin >> uuid;
    client_tcp_connection* check_item = new client_tcp_connection();
    json show_info;
    show_info["opcode"] = 44;
    show_info["username"] = username;
    show_info["token"] = password;
    show_info["uuid"] = uuid;
    
    check_item->client_send(show_info);
    check_item->client_recv();
    check_item->client_process_data(result);
    if(result){
        json p = check_item->show_recv_json();
        delete check_item;
        cout << "该物品现价格为：" << p["price"] << endl;
        cout << "该物品现主人为：" << p["owner"] << endl;
        
        bool djj = p["started_auction"];
        if(djj == false){
            cout << "该物品竞拍已结束或尚未开始" << endl;
            return 0;
        }
        
        cout << "输入您的出价：" << endl;
        cin.clear();
        fflush(stdin);
        cin >> price;
        
        bid["opcode"] = 6;
        bid["username"] = username;
        bid["token"] = password;
        bid["price"] = price;
        bid["uuid"] = uuid;
        
        client_tcp_connection* new_connection = new client_tcp_connection();
        new_connection->client_send(bid);
        new_connection->client_recv();
        new_connection->client_process_data(result);
        delete new_connection;
        
        if(result){
            cout << "出价成功 新价格为：" << price << endl;
        }
        else{
            cout << "出价失败" << endl;
        }
    }
    else{
        cout << "物品不存在" << endl;
        delete check_item;
        return 0;
    }
    return 0;
}

bool AdminLoadItems(void){
    json j;
    bool result;
    j["opcode"] = 7;
    j["username"] = username;
    j["token"] = password;
    j["started"] = 0;
    client_tcp_connection* tcp = new client_tcp_connection();
    tcp->client_send(j);
    tcp->client_recv();
    tcp->client_process_data(result);
    delete tcp;
    
    if(result == true){
        bool end_of_list = false;
        while(end_of_list == false){
            j["opcode"] = 7;
            j["username"] = username;
            j["token"] = password;
            j["started"] = 1;
            client_tcp_connection* new_tcp = new client_tcp_connection();
            new_tcp->client_send(j);
            new_tcp->client_recv();
            json djj = new_tcp->show_recv_json();
            cout << "物品UUID：" << djj["uuid"] << endl << "物品名：" << djj["name"] << endl << "物品持有者：" << djj["owner"] << endl << endl;
            end_of_list = djj["end"];
        }
    }
    
    return result;
}
