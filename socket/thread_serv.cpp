#include<iostream>
#include<winsock2.h>
#include<thread>
#include<vector>
#include<mutex>
#include<memory>
#include<queue>
#include<sstream>
#include<atomic>
using namespace std;

class SERV_sock{
private:    
    SOCKET serv_sock;       
    int serv_id;
    SOCKADDR_IN servaddr;            
    int port_;                  
    static const int clnts = 10;
    
    atomic<bool> server_running{true};

public:
    static int get_clnts() { return clnts; }
    static const int buffsize = 4096;

    void errorhandle() {
        cout << "Socket错误: " << WSAGetLastError() << endl;
    }
    
    struct clnt_sock{
        SOCKET clnt_sock_only = INVALID_SOCKET;
        int cknt_id = -1;
        SOCKADDR_IN clntaddr;
        atomic<bool> run{false}; 
        
        atomic<bool> thread_finished{true}; 
        
       
        ~clnt_sock() {
            if(clnt_sock_only != INVALID_SOCKET){
                closesocket(clnt_sock_only);
                clnt_sock_only = INVALID_SOCKET;
            }
        }
        
       
        void reset_for_reuse() {
            if(clnt_sock_only != INVALID_SOCKET){
                closesocket(clnt_sock_only);
                clnt_sock_only = INVALID_SOCKET;
            }
            run = false;
            thread_finished = true;
            cknt_id = -1;
            memset(&clntaddr, 0, sizeof(clntaddr));
        }
    };
    
    shared_ptr<clnt_sock> clnt_socks[clnts];
    vector<shared_ptr<thread>> uniptr;  
    mutex mtx;
    
    struct MessageBuffer {
        char data[buffsize];
        atomic<bool> has_data{false};  
    };
    shared_ptr<MessageBuffer> sendmessage[clnts];
    shared_ptr<MessageBuffer> recvmessage[clnts];
    
public:
    SERV_sock(int port):port_(port){
        serv_sock = socket(PF_INET,SOCK_STREAM,0);
        
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = PF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(port);
        
        if(bind(serv_sock,(SOCKADDR*)&servaddr,sizeof(servaddr))==SOCKET_ERROR){
            errorhandle();
            return;
        }
        if(listen(serv_sock,clnts)==SOCKET_ERROR){
            errorhandle();
        }
        
        for(int i = 0; i < clnts; i++){
            clnt_socks[i] = make_shared<clnt_sock>();
            sendmessage[i] = make_shared<MessageBuffer>();
            recvmessage[i] = make_shared<MessageBuffer>();
        }
    }    
    
    void accept_connect(){
        
        while(server_running){
            for(int i = 0; i < clnts && server_running; i++){
                
                if(clnt_socks[i]->thread_finished && !clnt_socks[i]->run){
                   
                    clnt_socks[i]->reset_for_reuse();
                    
                    int clntstr = sizeof(clnt_socks[i]->clntaddr);
                    
                   
                    clnt_socks[i]->clnt_sock_only = accept(serv_sock,
                                                          (SOCKADDR*)&clnt_socks[i]->clntaddr,
                                                          &clntstr);
                    
                    if(clnt_socks[i]->clnt_sock_only == INVALID_SOCKET){
                        if(WSAGetLastError() == WSAEINTR){
                            
                            return;
                        }
                        continue;
                    }
                    
                    clnt_socks[i]->cknt_id = i;
                    clnt_socks[i]->run = true;
                    clnt_socks[i]->thread_finished = false;
                    
                    cout << "客户端 " << i << " 连接成功" << endl;
                    
                   
                    auto clntmessage = make_shared<thread>([this, client_ptr = clnt_socks[i]]() {
                        
                        this->handle_client_for_ptr(client_ptr);
                    });
                    
                    clntmessage->detach();
                    uniptr.push_back(clntmessage);
                    
                   
                    this_thread::sleep_for(chrono::milliseconds(10));
                }
            }
            
           
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
    
   
    void handle_client_for_ptr(shared_ptr<clnt_sock> client_ptr){
        if(!client_ptr) return;
        
        int client_id = client_ptr->cknt_id;
        cout << "开始处理客户端 " << client_id << endl;
        
        
        auto send_thread = make_shared<thread>([this, client_ptr]() {
            this->send_message_worker_for_ptr(client_ptr);
        });
        
        
        auto recv_thread = make_shared<thread>([this, client_ptr]() {
            this->recv_message_worker_for_ptr(client_ptr);
        });
        
        send_thread->detach();
        recv_thread->detach();
        
        uniptr.push_back(send_thread);
        uniptr.push_back(recv_thread);
        
      
        while(client_ptr->run && server_running){
            
            char test;
            int result = recv(client_ptr->clnt_sock_only, &test, 1, MSG_PEEK);
            if(result == 0 || (result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)){
                client_ptr->run = false;
                break;
            }
            
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        
      
        client_ptr->thread_finished = true;
        cout << "客户端 " << client_id << " 处理完成，资源将自动释放" << endl;
        
       
    }
    
   
    void send_message_worker_for_ptr(shared_ptr<clnt_sock> client_ptr){
        if(!client_ptr) return;
        
        int client_id = client_ptr->cknt_id;
        auto send_buf = sendmessage[client_id];
        
        while(client_ptr && client_ptr->run && server_running){
            if(send_buf->has_data){
                
                int result = send(client_ptr->clnt_sock_only, 
                                 send_buf->data, 
                                 strlen(send_buf->data), 0);
                
                if(result == SOCKET_ERROR){
                    client_ptr->run = false;
                    break;
                }
                
                send_buf->has_data = false;
            }
            
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
    
   
    void recv_message_worker_for_ptr(shared_ptr<clnt_sock> client_ptr){
        if(!client_ptr) return;
        
        int client_id = client_ptr->cknt_id;
        auto recv_buf = recvmessage[client_id];
        char temp_buffer[buffsize];
        
        
        u_long mode = 1;
        ioctlsocket(client_ptr->clnt_sock_only, FIONBIO, &mode);
        
        while(client_ptr && client_ptr->run && server_running){
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(client_ptr->clnt_sock_only, &readfds);
            
            timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            
            int ready = select(0, &readfds, NULL, NULL, &timeout);
            
            if(ready > 0){
                int recv_len = recv(client_ptr->clnt_sock_only, 
                                   temp_buffer, buffsize - 1, 0);
                
                if(recv_len > 0){
                    temp_buffer[recv_len] = '\0';
                    
                    lock_guard<mutex> lock(mtx);
                    strcpy_s(recv_buf->data, temp_buffer);
                    recv_buf->has_data = true;
                    
                    cout << "收到客户端" << client_id << "的消息: " 
                         << recv_buf->data << endl;
                }
                else if(recv_len == 0){
                    
                    client_ptr->run = false;
                    break;
                }
            }
            else if(ready < 0){
                
                if(WSAGetLastError() != WSAEINTR){
                    client_ptr->run = false;
                    break;
                }
            }
        }
        
       
        u_long mode_blocking = 0;
        ioctlsocket(client_ptr->clnt_sock_only, FIONBIO, &mode_blocking);
    }
    
    
    void handle_client(int client_id){
        if(client_id >= 0 && client_id < clnts){
            handle_client_for_ptr(clnt_socks[client_id]);
        }
    }
    
    void send_message(int client_id){
        if(client_id < 0 || client_id >= clnts){
            cout << "无效的客户端ID" << endl;
            return;
        }
        
        if(!clnt_socks[client_id]->run){
            cout << "未连接该id的客户端，请重试" << endl;
            return;
        }
        
        cout << "请输入要发送给客户端 " << client_id << " 的文字:" << endl;
        
        auto send_buf = sendmessage[client_id];
        char input[buffsize];
        cin.getline(input, buffsize);
        
        if(strcmp(input, "exit") == 0){
            EXIT_sock(client_id);
            return;
        }
        
        lock_guard<mutex> lock(mtx);
        strcpy_s(send_buf->data, input);
        send_buf->has_data = true;
    }
    
    void EXIT_sock(int id){
        if(id >= 0 && id < clnts && clnt_socks[id]){
           
            clnt_socks[id]->run = false;
            
            
            cout << "客户端 " << id << " 已标记为断开" << endl;
        }
        else{
            cout << "错误: 无效的客户端ID " << id << endl;
        }
    }
    
    
    void stop_server(){
        server_running = false;
        
      
        if(serv_sock != INVALID_SOCKET){
            closesocket(serv_sock);
            serv_sock = INVALID_SOCKET;
        }
        
       
        for(int i = 0; i < clnts; i++){
            if(clnt_socks[i]){
                clnt_socks[i]->run = false;
            }
        }
        
      
        uniptr.clear();
        
        cout << "服务器已停止" << endl;
    }
    
    ~SERV_sock(){
        stop_server();
        
        
        this_thread::sleep_for(chrono::milliseconds(200));
    }
};


void start_test_client(int client_id){
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(8080);
    
    if(connect(sock, (sockaddr*)&servAddr, sizeof(servAddr)) == 0){
        cout << "测试客户端 " << client_id << " 连接成功" << endl;
        
      
        thread([sock, client_id](){
            char buffer[4096];
            while(true){
                int recv_len = recv(sock, buffer, sizeof(buffer)-1, 0);
                if(recv_len > 0){
                    buffer[recv_len] = '\0';
                    cout << "测试客户端 " << client_id << " 收到: " << buffer << endl;
                }
                else if(recv_len == 0){
                    cout << "测试客户端 " << client_id << " 服务器断开连接" << endl;
                    break;
                }
                else if(WSAGetLastError() == WSAECONNRESET){
                    cout << "测试客户端 " << client_id << " 连接被重置" << endl;
                    break;
                }
            }
        }).detach();
        
       
        thread([sock, client_id](){
            int count = 0;
            while(true){
                string msg = "Hello from client " + to_string(client_id) + " - " + to_string(count++);
                send(sock, msg.c_str(), msg.length(), 0);
                this_thread::sleep_for(chrono::seconds(3));
            }
        }).detach();
        
        
        this_thread::sleep_for(chrono::seconds(30));
    }
    else{
        cout << "测试客户端 " << client_id << " 连接失败" << endl;
    }
    
    closesocket(sock);
    WSACleanup();
}

int main() {
    cout << "=== 多线程TCP服务器（智能指针自动管理）===" << endl;
    
   
    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2,2),&wsadata) != 0){  
        cout << "Winsock初始化错误" << endl;
        return 1;
    }
    
    try {
       
        int port = 8080;
        SERV_sock server(port);
        
        cout << "服务器创建成功，端口: " << port << endl;
        cout << "最大客户端数: " << SERV_sock::get_clnts() << endl;
        cout << "智能指针管理: 线程结束自动释放资源" << endl;
        
       
        cout << "\n请选择运行模式:" << endl;
        cout << "1. 自动测试模式 (启动测试客户端)" << endl;
        cout << "2. 交互模式 (手动发送消息)" << endl;
        cout << "3. 简单模式 (仅接受连接)" << endl;
        cout << "请输入选择 (1-3): ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
       
        thread server_thread([&server](){
            cout << "服务器线程开始运行..." << endl;
            server.accept_connect();
            cout << "服务器线程结束" << endl;
        });
        
        if(choice == 1){
           
            cout << "\n启动自动测试..." << endl;
            cout << "将启动3个测试客户端" << endl;
            
           
            thread([](){ start_test_client(0); }).detach();
            thread([](){ start_test_client(1); }).detach();
            thread([](){ start_test_client(2); }).detach();
            
            cout << "\n测试客户端已启动，运行30秒..." << endl;
            cout << "观察控制台输出，验证功能:" << endl;
            cout << "1. 客户端连接成功" << endl;
            cout << "2. 服务器收到客户端消息" << endl;
            cout << "3. 客户端断开后自动清理" << endl;
            cout << "4. 新连接可以重用位置" << endl;
            
            this_thread::sleep_for(chrono::seconds(30));
            
            cout << "\n测试完成，停止服务器..." << endl;
            server.stop_server();
        }
        else if(choice == 2){
           
            cout << "\n=== 交互模式 ===" << endl;
            cout << "可用命令:" << endl;
            cout << "send [id]  - 向客户端发送消息 (0-" << SERV_sock::get_clnts()-1 << ")" << endl;
            cout << "exit       - 退出服务器" << endl;
            cout << "help       - 显示帮助" << endl;
            
            bool running = true;
            while(running){
                cout << "\n服务器> ";
                string input;
                getline(cin, input);
                
                if(input == "help"){
                    cout << "\n智能指针自动管理功能:" << endl;
                    cout << "1. 线程结束时自动释放内存" << endl;
                    cout << "2. 套接字自动关闭" << endl;
                    cout << "3. 客户端位置自动重用" << endl;
                    cout << "4. 无需手动析构函数管理" << endl;
                }
                else if(input.substr(0,4) == "send"){
                    stringstream ss(input);
                    string cmd;
                    int client_id;
                    ss >> cmd >> client_id;
                    
                    if(client_id >= 0 && client_id < SERV_sock::get_clnts()){
                        server.send_message(client_id);
                    }
                    else{
                        cout << "客户端ID必须在 0-" << SERV_sock::get_clnts()-1 << " 之间" << endl;
                    }
                }
                else if(input == "exit"){
                    running = false;
                    server.stop_server();
                }
                else if(!input.empty()){
                    cout << "输入 'help' 查看帮助" << endl;
                }
            }
        }
        else if(choice == 3){
           
            cout << "\n简单模式运行中..." << endl;
            cout << "等待客户端连接..." << endl;
            cout << "按Ctrl+C停止" << endl;
            
            
            while(true){
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
        
       
        if(server_thread.joinable()){
            server_thread.join();
        }
        
        cout << "\n服务器已完全停止" << endl;
    }
    catch(const exception& e){
        cout << "错误: " << e.what() << endl;
    }
    
    WSACleanup();
    
    cout << "\n程序结束，按Enter退出...";
    cin.get();
    
    return 0;
}