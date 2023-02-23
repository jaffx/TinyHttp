//
// Created by lyn on 2023/2/23.
//

#ifndef MAIN_XHTTP_RESPONSE_H
#define MAIN_XHTTP_RESPONSE_H

#include <iostream>
#include <unordered_map>
#include <string>

#define HTTP_REQUEST_LINE_SIZE 8192    // 客户端请求行长度上限
#define HTTP_URL_SIZE 2048             // url长度上限
#define CONNECT_LIFE_TIME 12           // 连接的声明周期，单位秒
#define CONNECT_SCAN_TIME 1            // 连接扫描周期


namespace xyq {
    class xhttp_response {

        std::string buffer;

    public:
        const std::string http_version = "HTTP/1.1";
        int status_code = 200;
        std::string message = "OK";
        std::unordered_map<std::string, std::string> rsp_header;
        std::string rsp_content;

        xhttp_response();

        void not_found();

        void ok();

        void bad_request();

        void time_out();

        std::string to_string();

        void set_message(std::string message);

        void set_status_code(std::string status);

        void set_status_code(int status);

        void set_header(std::string, std::string);

        void operator=(const xhttp_response &);

        void set_content(std::string content);

        void add_content(std::string content);

        friend class xhttp_connect;
    };

};
#endif //MAIN_XHTTP_RESPONSE_H
