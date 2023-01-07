#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#define MYIP "127.0.0.1"
#define PORT 1120

using namespace std;
int main()
{
    int clnt_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(MYIP);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    int conret = connect(clnt_sock, (struct sockaddr *)&addr, sizeof(addr));
    cout<<"Conret:"<<conret<<endl;

    char writebuf[1024] = "GET / HTTP/1.1\r\n";
    int write_ret = write(clnt_sock, writebuf, sizeof(writebuf));
    if (write_ret == -1){
        cout<<"发送失败"<<endl;
    }
    else{
        cout<<"成功字节数："<<write_ret<<endl;
    }
}