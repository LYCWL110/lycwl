#include<iostream>
#include<string>
#include<winsock2.h>
#define buff_size 1024

using namespace std;

void errorhanding(const string str);
int main(int argc,char* argv[]){
    WSAData wsadata;
    SOCKET clnt_sock;
    SOCKADDR_IN servaddr;

    if(argc != 3){
        cout<<"Usage : "<<argv[0]<<" <IP> <PORT>"<<endl;
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) errorhanding("wsadata error");

    clnt_sock = socket(PF_INET,SOCK_STREAM,0);
    if(clnt_sock == INVALID_SOCKET) errorhanding("socket error");

    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port  = htons(atoi(argv[2]));

    if(connect(clnt_sock,(SOCKADDR*)&servaddr,sizeof(servaddr))==INVALID_SOCKET) 
        errorhanding("connect error");
    cout<<"已连接上主机"<<endl;
    
    char message[buff_size];

    while(true){
        
        cout << "请输入消息: ";
        cin.getline(message, buff_size);
        
        
        if(strcmp(message, "quit") == 0) break;
        
       
        send(clnt_sock, message, strlen(message), 0);
        
        int strlen = recv(clnt_sock, message, buff_size-1, 0);
        if(strlen == -1) errorhanding("read error");
        
        
        message[strlen] = '\0';
        cout<<"服务器: "<<message<<endl;
    }
    
    closesocket(clnt_sock);
    WSACleanup();
    return 0; 
}

void errorhanding(const string str){
    cout<<str<<endl;
    exit(1);
}