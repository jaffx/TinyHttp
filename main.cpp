#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <thread>
#include <arpa/inet.h>
#include "xhttp.h"
using namespace std;
#define PORT 1120
#define MYIP "127.0.0.1"

int main()
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
        socklen_t clnt_len = sizeof(clnt_addr);
    if ((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout << "Socket初始化失败！" << endl;
    }
    // 设置服务器Socket地址
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(MYIP);
    cout << "[Server]";
    show_sockaddr_in(serv_addr);
    // 绑定服务器地址
    if (::bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        cout << "该地址已经被使用，bind失败！" << endl;
    }

    if (listen(serv_sock, 5)==-1){
        cout<<"该端口已被监听,listen失败！"<<endl;
    }
    while (clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_len))
    {
        cout << "####################" << endl;
        cout << "[Clinet]";
        show_sockaddr_in(clnt_addr);
        thread th(do_http_request, clnt_sock);
        th.detach();
    }
    close(serv_sock);
}