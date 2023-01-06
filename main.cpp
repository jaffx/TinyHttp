#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include "xhttp.h"
using namespace std;

#define PORT 1120
#define MYIP "127.0.0.1"

int main()
{
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    cout<<"server socket:"<<serv_sock<<endl;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(MYIP);
    cout<<"IP  :"<<MYIP<<endl;
    cout<<"PORT:"<<PORT<<endl;
    bind(serv_sock, (struct sockaddr *)&addr, sizeof(addr));

    listen(serv_sock, 20);
    cout<<"Get Clinet Connect!"<<endl;
    struct sockaddr_in clnt_addr;
    memset(&clnt_addr, 0, sizeof(clnt_addr));
    socklen_t clnt_len = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_len);

    do_http_request(clnt_sock);
    close(clnt_sock);
    close(serv_sock);

}