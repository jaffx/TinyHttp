#include "xhttp.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>


void do_http_request(int sock){
    struct sockaddr_in clnt_addr;
    socklen_t clnt_sock_len = sizeof(clnt_addr);
    char inbuf[1024], outbuf[1024];
    read(sock, inbuf, sizeof(inbuf));
    xhttp_req req_info = get_http_request_info(inbuf);
    show_xhttp_req(req_info);
}

xhttp_req get_http_request_info(const char *__http_req){
    xhttp_req req;
    int idx = 0;
    int si =0;
    while(__http_req[idx]!=' '){
        req.method[si++]=__http_req[idx++];
    }
    req.method[si]='\0';
    si =0;
    while(__http_req[idx]!=' '){
        req.url[si++]=__http_req[idx++];
    }
    req.url[si]='\0';
    si =0;
    while(__http_req[idx]!='\r' and __http_req[idx+1]!='\n'){
        req.version[si++]=__http_req[idx++];
    }
    req.version[si]='\0';
    return req;

}

void show_xhttp_req(xhttp_req &req){
    using namespace std;
    cout<<req.method<<' '<<req.url<<' '<<req.version<<endl;
}

