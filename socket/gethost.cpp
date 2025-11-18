#include<iostream>
#include<winsock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main(int argc, char* argv[]){

    WSADATA wsadata;  
    //hostent* host;
    hostent* host1;
    SOCKADDR_IN addr;

    if(argc != 2){
        cout << "Usage: " << argv[0] << " <hostname>" << endl;
        cout << "Example: " << argv[0] << " www.baidu.com" << endl;
        exit(1);
    }
    
    if(WSAStartup(MAKEWORD(2,2), &wsadata) != 0){  
        cout << "WSAStartup failed!" << endl;
        exit(1);
    }

    /*host = gethostbyname(argv[1]);
    if(!host) {
        cout << "Failed to resolve hostname: " << argv[1] << endl;
        cout << "Error code: " << WSAGetLastError() << endl;
        WSACleanup();
        exit(1);
    }

    cout << "Official name: " << host->h_name << endl;
    
    cout << "Aliases: ";
    if(host->h_aliases[0] != NULL) {
        for(int i = 0; host->h_aliases[i]; i++){
            cout << host->h_aliases[i] << " ";
        }
    } else {
        cout << "None";
    }
    cout << endl;
    
    cout << "Address type: " << (host->h_addrtype == AF_INET ? "AF_INET" : "AF_INET6") << endl;
    cout << "Address length: " << host->h_length << " bytes" << endl;
    
    cout << "IP addresses: ";
    for(int i = 0; host->h_addr_list[i]; i++){
       
        struct in_addr addr;
        addr.s_addr = *(u_long*)host->h_addr_list[i];
        cout << inet_ntoa(addr) << " "; 
    }
    cout << endl;*/


    memset(&addr,0,sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    if(addr.sin_addr.s_addr == INADDR_NONE) {
        cout << "Invalid IP address: " << argv[1] << endl;
        WSACleanup();
        exit(1);
    }
    host1 = gethostbyaddr((char*)&addr.sin_addr,4,AF_INET);
    if(!host1){
        exit(1);
    }
    cout<<host1->h_name;
    for(int i =0;host1->h_aliases[i];i++){
        cout<<host1->h_aliases[i];
    }
    cout<<host1->h_addrtype;
    cout << "IP addresses: ";
    for(int i = 0; host1->h_addr_list[i]; i++){
        
        struct in_addr* ip_addr = (struct in_addr*)host1->h_addr_list[i];
        cout << inet_ntoa(*ip_addr);
        if(host1->h_addr_list[i+1]) cout << ", ";
    }
    cout << endl;
    WSACleanup();
    return 0;
}