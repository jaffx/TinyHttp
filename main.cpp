#include "xServer/xServer.h"
#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;
xyq::xhttp_response func1(xyq::xhttp_request req){
    xyq::xhttp_response rsp;
    rsp.ok();
    rsp.rsp_content = "找啊找啊找朋友！！";
    return rsp;
}
int main()
{
    xyq::xhttp_server xs("TCP", "127.0.0.1", 1120);
    xs.add_path("/",func1);
    xs.run();
    // auto &&http_con = xs.xs_get_connect();
    // auto && clnt_req = http_con.get_http_request();
    // xs.xs_close();
}