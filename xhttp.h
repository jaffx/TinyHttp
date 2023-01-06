#ifndef XHTTP_H
#define XHTTP_H
#include <unordered_map>

struct xhttp_request_info{
    char method[8];
    char url[64];
    char version[32];    
};
typedef xhttp_request_info xhttp_req;

void do_http_request(int sock);
xhttp_req get_http_request_info(const char*);
void show_xhttp_req(xhttp_req&);

#endif