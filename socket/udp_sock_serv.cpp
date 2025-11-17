#include<iostream>
#include<winsock2.h>
#define buffsize 1024
using namespace std;

void errorhanding(const string str); 

int main(int argc, char* argv[]){
    WSADATA wsadata;
    SOCKET serv_sock;
    char message[buffsize]; 
    SOCKADDR_IN servaddr, clntaddr;
    int strlenth;
    int clntlen; 

    if(argc != 2){
        cout << "用法: " << argv[0] << " <端口号>" << endl;
        exit(1);
    }

   
    if(WSAStartup(MAKEWORD(2,2), &wsadata) != 0){ 
        errorhanding("WSAStartup error");
    }

    
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(serv_sock == INVALID_SOCKET){
        errorhanding("socket error");
    }

    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

   
    if(bind(serv_sock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR){
        errorhanding("bind error");
    }

    cout << "UDP服务器启动，监听端口: " << argv[1] << endl;

   
    while(true){
        clntlen = sizeof(clntaddr);
        
       
        strlenth = recvfrom(serv_sock, message, buffsize - 1, 0, 
                           (SOCKADDR*)&clntaddr, &clntlen);
        
        if(strlenth == SOCKET_ERROR){
            cout << "recvfrom error: " << WSAGetLastError() << endl;
            continue;
        }
        
       
        message[strlenth] = '\0';
        cout << "收到消息: " << message << endl;
        
       
        if(sendto(serv_sock, message, strlenth, 0, 
                 (SOCKADDR*)&clntaddr, clntlen) == SOCKET_ERROR){
            cout << "sendto error: " << WSAGetLastError() << endl;
        }
    }

    closesocket(serv_sock);
    WSACleanup();
    return 0;
}


void errorhanding(const string str){
    cout << str << " 错误代码: " << WSAGetLastError() << endl;
    exit(1);
}