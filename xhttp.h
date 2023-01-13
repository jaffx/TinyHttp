#ifndef XHTTP_H
#define XHTTP_H
#include <unordered_map>
#include <optional>

#include "xyq/xyq.h"
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include<sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <utility>
#include <unordered_map>

#define URL_PARAM_KLEN 256
#define URL_PARAM_VLEN 1024
#define URL_PATH_LEN 2048
#define URL_MAX_LEN 2048

using std::cout, std::endl, std::cerr;
using std::pair;
using std::string;

typedef pair<string, string> kv_pair;

struct xhttp_req_line
{
    char method[32];
    char url[2048];
    char version[32];
};

struct xhttp_url_info
{
    std::unordered_map<string, string> params;
    string path;
};
typedef xhttp_req_line req_line;

void do_http_request(int sock);
std::optional<req_line> get_http_request_line(const char *);
int get_line(int sock, char *buf, int size);
std::optional<kv_pair> get_key_value(const char *buf);

void show_xhttp_req_line(req_line &);
void show_sockaddr_in(const struct sockaddr_in &);

int read_html_file(std::fstream & file, char *buffer, int bufsize);

int response_not_found(int);
int response_html(int sock, const char* html_src);
std::optional<struct xhttp_url_info> ana_url(const char* url);

#endif