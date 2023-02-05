#ifndef X_SERVER_H
#define X_SERVER_H
#include "xyq.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <cstring>
#include <string>
#include <algorithm>
#include <utility>
#include <unistd.h>
#include <thread>
#include <unordered_map>
#define CLNT_CONNECT_BUFFER_SIZE 65536 // 客户端链接写buffer长度
#define HTTP_REQUEST_LINE_SIZE 8192    // 客户端请求行长度上限
#define HTTP_URL_SIZE 2048             // url长度上限
#define CONNECT_LIFE_TIME 12           // 连接的声明周期，单位秒
#define CONNECT_SCAN_TIME 1            // 连接扫描周期

namespace xyq
{
    // 类声明
    class xconnect_base;
    class xhttp_response;
    class xhttp_request;
    class xhttp_server;
    class xserver_exception;
    typedef xhttp_response (*xhttp_rsp_func)(xhttp_request);

    // 设置参数及预定义值
    extern std::string template_path;
    enum xserver_status
    {
        NOT_INIT = 0,       // 尚未初始化
        SOCK_READY = 1,     // socket初始化成功
        BIND_READY = 2,     // bind成功
        LISTENING = 3,      // listen中
        SOCK_CLOSE = 4,     // sock关闭
        XSERVER_ERROR = -1, // 关键操作失败，server不可用
    };

    enum xconnect_status
    {
        XCONNECT_ERROR = -1, // 未使用
        WAITING = 0,         // 未使用
        READY = 1,           // 未使用
        RUNNING = 2,         // 正在执行业务逻辑
        TIME_OUT = 3,        // 超时
        FINISH = 4,          // 完成处理，正在向客户端发送数据
        RESPONSE_FINISH = 5, // 数据发送已完成
        CLOSE = 6,           // 连接已关闭
    };
    // 基类声明
    class xserver_base
    {
    protected:
        int __socket;
        struct sockaddr_in __addr;
        std::string __ip, __protocal;
        __uint16_t __port;
        xserver_status __status, __last_status;

    public:
        xserver_base(std::string protocal, std::string ip, __uint16_t port);
        int xs_bind();
        int xs_listen();
        xconnect_base xs_get_connect();
        virtual void run() = 0;
        void xs_close();
        inline void update_status(xserver_status status);
        virtual void show_info() const;
    };

    class xconnect_base
    {
    protected:
        int __socket;
        bool ready;
        struct sockaddr_in __addr;
        std::string __ip;
        __uint16_t __port;
        char __buffer[CLNT_CONNECT_BUFFER_SIZE];
        xconnect_status __status, __last_status;
        inline void update_status(xconnect_status) noexcept;

    public:
        xconnect_base(int, const struct sockaddr_in &);
        int xc_write(const std::string &);
        int xc_write(const char *);
        int xc_read(size_t size);
        void xc_close();
        std::string get_content() const;
        int get_line();
        virtual void show_info() const;
        virtual void run() = 0;
    };

    // http服务声明

    class xhttp_request
    {
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
    };

    class xhttp_response
    {

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

    class xhttp_connect : public xconnect_base
    {
    private:
        xhttp_server *__server = NULL;
        uint64_t __cid = 0;
        pthread_t __pid;
        std::chrono::system_clock::time_point start_time_point;
        void set_cid(uint64_t) noexcept;
        void set_pid(pthread_t) noexcept;

    public:
        friend xhttp_server;
        xhttp_connect(int socket, const struct sockaddr_in &addr, xhttp_server *server = NULL);
        xhttp_connect(int socket, const struct sockaddr_in &addr, xhttp_server *server, uint64_t id);
        ~xhttp_connect();
        std::tuple<std::string, std::string, std::string> ana_req_line();
        std::pair<std::string, std::string> ana_key_value();

        virtual void show_info();

        xhttp_request get_http_request();
        void do_http__();
        void put_http_response(xhttp_response &);
        void time_out();
        virtual void run();
        virtual void xc_close();
    };

    class xhttp_server : public xserver_base
    {
    private:
        std::unordered_map<std::string, xhttp_rsp_func> rsp_mapping;
        uint64_t __connect_count = 0;
        uint64_t get_connect_id() noexcept;
        void add_connect(xhttp_connect *);
        int remove_connect(uint64_t pid);
        void accept_http_connect();
        void manage_http_connect();
        std::unordered_map<uint64_t, xhttp_connect *> connects;

    public:
        friend xhttp_connect;
        xhttp_server(std::string ip, size_t port) : xserver_base("tcp", ip, port)
        {
        }
        ~xhttp_server() {}
        xhttp_connect *xs_get_connect();
        void add_path(std::string, xhttp_rsp_func);
        xhttp_rsp_func get_path_mapping(std::string);
        virtual void run();
    };

    class xhttp_render
    {
    private:
        std::string path = "";
        std::fstream reader;
        std::string result;
        std::unordered_map<std::string, std::string> maps;

    public:
        xhttp_render(std::string path, std::unordered_map<std::string, std::string> &);
        void scan();
        void trans();
        bool enable();
        std::string get_result();
    };
    // 服务异常类声明
    class xserver_exception : public std::exception
    {
        std::string message;

    public:
        xserver_exception(std ::string &message) : exception()
        {
            this->message = message;
        }
        xserver_exception(const char *message) : exception()
        {
            this->message = message;
        }
        virtual std::string what()
        {
            // std::cout << "[" << __FILE__ << ":" << __LINE__ << "]##  " << message << std::endl;
            return this->message;
        }
    };

    class xconnect_exception : public std::exception
    {
        std::string message;

    public:
        xconnect_exception(std ::string &message) : exception()
        {
            this->message = message;
        }
        xconnect_exception(const char *message) : exception()
        {
            this->message = message;
        }
        virtual std::string what()
        {
            // std::cout << "[" << __FILE__ << ":" << __LINE__ << "]##  " << message << std::endl;
            return this->message;
        }
    };
    // 函数定义
    xhttp_response render(std::string path);
    xhttp_response render(std::string path, std::unordered_map<std::string, std::string> &);
}
#endif