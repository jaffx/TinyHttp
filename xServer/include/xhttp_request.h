//
// Created by lyn on 2023/2/23.
//

#ifndef MAIN_XHTTP_REQUEST_H
#define MAIN_XHTTP_REQUEST_H

#include <string>
#include <unordered_map>
#include "xyq.h"
#include <iostream>

using std::cout, std::endl;
using std::string;


#define CLNT_CONNECT_BUFFER_SIZE 65536 // 客户端链接写buffer长度
#define HTTP_REQUEST_LINE_SIZE 8192    // 客户端请求行长度上限
#define HTTP_URL_SIZE 2048             // url长度上限
#define CONNECT_LIFE_TIME 12           // 连接的声明周期，单位秒
#define CONNECT_SCAN_TIME 1            // 连接扫描周期

namespace xyq {
    class xhttp_request {
    protected:
        bool enable = true;
        std::string url;
        std::string ip;
        std::string method;
        std::string http_version;
        std::string path;
        std::string req_content;

        void set_header(std::string key, std::string value);

        void set_header(const std::pair<std::string, std::string> &kv);

        void ana_url();

        std::string buffer;

    public:
        int port;
        std::unordered_map<std::string, std::string> req_header;
        std::unordered_map<std::string, std::string> params;

        std::string get_header(std::string);

        bool get_enable() const;

        std::string get_method() const;

        std::string get_ip() const;

        std::string get_version() const;

        std::string get_path();

        std::string text();

        friend class xhttp_connect;

        void show_info() const ;
    };
};


#endif //MAIN_XHTTP_REQUEST_H
