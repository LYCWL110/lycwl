#include<iostream>
#include<winsock2.h>
#include<string>
#include<cstdlib>  
#include<cstring> 
#define buffsize 1024

using namespace std;
void errorhand(const string);

int main(int argc, char* argv[]) {  
    if(argc != 2) {
        cout << "用法: " << argv[0] << " <端口号>" << endl;
        return 1;
    }
    
    WSADATA wsaDATA;
    SOCKET serv_sock, clnt_sock;
    SOCKADDR_IN servaddr, clntaddr;
    int str_len;
    char message[buffsize];  
    
    if(WSAStartup(MAKEWORD(2,2), &wsaDATA) != 0) {
        errorhand("WSAStartup error");
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == INVALID_SOCKET) {
        errorhand("socket creation error");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        errorhand("bind error");
    }

    if(listen(serv_sock, 5) == SOCKET_ERROR) {
        errorhand("listen error");
    }

    cout << "服务器启动，监听端口: " << argv[1] << endl;

    int szclntaddr = sizeof(clntaddr);
    
   
        clnt_sock = accept(serv_sock, (SOCKADDR*)&clntaddr, &szclntaddr);
        if(clnt_sock == INVALID_SOCKET) 
            errorhand("accept error");
        
        cout << "客户端连接成功!" << endl;
        
       
        while((str_len = recv(clnt_sock, message, buffsize-1, 0)) > 0) {
            
            message[str_len] = '\0';
            
            cout << "收到: " << message<<endl;
            cout<<"请输入消息"<<endl;

            cin.getline(message,buffsize);
            if(strcmp(message, "quit") == 0) break;
            
            send(clnt_sock, message, strlen(message), 0);
            
        }
        
       
        if(str_len == 0) {
            cout << "客户端断开连接" << endl;
        } else {
            cout << "接收错误" << endl;
        }
        
        closesocket(clnt_sock);
    
    
    closesocket(serv_sock);
    WSACleanup();
    return 0;
}

void errorhand(const string str){
    cout << str << endl;
    exit(1);
}