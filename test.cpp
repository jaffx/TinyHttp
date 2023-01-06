#include "xhttp.h"
#include <iostream>

using namespace std;

int main(){
    char http_req[] = "GET /root HTTP/1.1\r\n";
    xhttp_req ret = get_http_request_info(http_req);
    show_xhttp_req(ret);
}