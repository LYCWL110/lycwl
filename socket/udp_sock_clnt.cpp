#include<iostream>
#include<winsock2.h>
#include<string>
#include<cstring>
#define buff_size 1024

using namespace std;

void errorhanding(const string str);

int main(int argc, char* argv[]){
    WSADATA wsadata;
    SOCKET clnt_sock;
    SOCKADDR_IN servaddr;
    int strlen1;
    char message[buff_size];

    if(argc != 3){
        cout << "Usage : " << argv[0] << " <IP> <PORT>" << endl;
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2,2), &wsadata) != 0){
        errorhanding("WSAStartup error");
    }

    clnt_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(clnt_sock == INVALID_SOCKET){
        errorhanding("socket error");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    
    if(connect(clnt_sock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR){
        errorhanding("connect error");
    }

    cout << "连接到服务器 " << argv[1] << ":" << argv[2] << endl;
    cout << "输入消息(输入q退出):" << endl;

    while(true){
        cout << "> ";
        cin.getline(message, buff_size - 1);
        
        if(strcmp(message, "q") == 0){
            break;
        }

       
        int send_len = send(clnt_sock, message, strlen(message), 0);
        
        if(send_len == SOCKET_ERROR){
            cout << "send error: " << WSAGetLastError() << endl;
            continue;
        }

       
        strlen1 = recv(clnt_sock, message, buff_size - 1, 0);
        
        if(strlen1 == SOCKET_ERROR){
            cout << "recv error: " << WSAGetLastError() << endl;
            continue;
        }

        message[strlen1] = '\0';
        cout << "服务器回复: " << message << endl;
    }

    closesocket(clnt_sock);
    WSACleanup();
    cout << "客户端已关闭" << endl;
    return 0;
}

void errorhanding(const string str){
    cout << str << " 错误代码: " << WSAGetLastError() << endl;
    exit(1);
}