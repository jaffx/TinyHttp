#include "xServer.h"

std::string xyq::template_path = "template/"; // html模板所在文件夹
xyq::xserver_base::xserver_base(std::string protocal, std::string ip, __uint16_t port)
{
    /*
    @brief
        xserver_base类构造函数
    @param
        protocal TCP或者是UDP
    */
    this->__ip = ip;
    this->__port = port;
    this->__protocal = protocal;
    transform(__protocal.begin(), __protocal.end(), __protocal.begin(), tolower);
    if (this->__protocal == "tcp")
        this->__socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    else if (protocal == "udp")
        this->__socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    else
    {
        update_status(XSERVER_ERROR);
        throw xyq::xserver_exception("Socket协议必须为tcp/TCP或upd/UDP");
    }
    this->update_status(SOCK_READY);
    memset(&this->__addr, 0, sizeof(this->__addr));
    this->__addr.sin_family = AF_INET;
    this->__addr.sin_port = htons(port);
    this->__addr.sin_addr.s_addr = inet_addr(ip.data());
}

int xyq::xserver_base::xs_bind()
{
    int bind_ret = -1;
    int try_count = 0;
    switch (this->__status)
    {
    case (SOCK_READY):
        // socket 就绪，可以进行bind操作
        while (bind_ret != 0 and try_count < 5)
        {
            bind_ret = bind(this->__socket, (struct sockaddr *)(&this->__addr), sizeof(this->__addr));
            try_count++;
            if (bind_ret == -1)
            {
                this->__port++;
                this->__addr.sin_port = ntohs(this->__port);
            }
        }
        if (bind_ret == -1)
        {
            this->update_status(XSERVER_ERROR);
            xyq::xserver_exception("bind失败，该端口已被占用");
            return -1;
        }
        // bind成功
        this->update_status(BIND_READY);
        return 0;
        break;
    case (XSERVER_ERROR):
        throw xyq::xserver_exception("服务器发生错误，无法进行BIND操作");
        break;
    default:
        break;
    }
    return -1;
}
int xyq::xserver_base::xs_listen()
{
    switch (this->__status)
    {
    case XSERVER_ERROR:
        // server出现错误
        throw xyq::xserver_exception("服务器出现错误，无法进行Listen操作");
        break;
    case SOCK_READY:
        // socket初始化成功，尚未bind，进行bind操作
        this->xs_bind();
    case BIND_READY:
        // bind完成，可以直接listen
        if (listen(this->__socket, 20) == 0)
        {
            // listen成功
            this->update_status(LISTENING);
            return 0;
        }
        else
        {
            // listen失败
            this->update_status(XSERVER_ERROR);
            throw xyq::xserver_exception("该端口已被监听，listen失败");
            return -1;
        }
        break;
    default:
        break;
    }
    return -1;
}
void xyq::xserver_base::xs_close()
{
    if (close(this->__socket))
    {
        // 成功关闭socket
        this->update_status(SOCK_CLOSE);
    }
    else
    {
        // 关闭socket失败
        this->update_status(XSERVER_ERROR);
    }
}
// xyq::xconnect_base xyq::xserver_base::xs_get_connect()
// {
//     if (this->__status != LISTENING)
//         this->xs_listen();
//     sockaddr_in clnt_addr;
//     socklen_t clntaddr_len = sizeof(clnt_addr);
//     int clnt_sock = accept(this->__socket, (struct sockaddr *)(&clnt_addr), &clntaddr_len);
//     xconnect_base clnt_con(clnt_sock, clnt_addr);
//     return clnt_con;
// }
xyq::xhttp_connect *xyq::xhttp_server::xs_get_connect()
{
    /*
        @brief 获取来自客户端的连接
    */
    // 如果没有进行listen，则进行listen操作
    if (this->__status != LISTENING)
        this->xs_listen();

    sockaddr_in clnt_addr;
    socklen_t clntaddr_len = sizeof(clnt_addr);
    int clnt_sock = accept(this->__socket, (struct sockaddr *)(&clnt_addr), &clntaddr_len);
    uint64_t connect_id = this->get_connect_id();
    xhttp_connect *clnt_con = new xhttp_connect(clnt_sock, clnt_addr, this, connect_id);
    return clnt_con;
}
void xyq::xhttp_connect::run()
{
    /*
    http连接开始工作
    */
    std::thread th(&xhttp_connect::do_http__, this);
    this->set_pid(th.native_handle());
    th.detach();
}
void xyq::xhttp_server::add_path(std::string path, xyq::xhttp_rsp_func rsp_function)
{
    /*
    @brief
        向服务器添加url和业务处理逻辑映射
    */
    if (path.size() == 0)
        return;
    if (this->rsp_mapping.find(path) != this->rsp_mapping.end())
        return;
    this->rsp_mapping[path] = rsp_function;
}
xyq::xhttp_rsp_func xyq::xhttp_server::get_path_mapping(std::string url)
{
    /*
    @brief  根据url获取处理逻辑函数
    */
    if (this->rsp_mapping.find(url) == this->rsp_mapping.end())
        return NULL;
    else
        return this->rsp_mapping[url];
}
void xyq::xhttp_server::accept_http_connect()
{
    /*
    @brief  接受http链接
    */
    while (true)
    {
        // 等待服务器连接请求
        try
        {
            auto clnt_con = this->xs_get_connect();
            clnt_con->run();
        }
        catch (xconnect_exception e)
        {
            std::cout << e.what() << std::endl;
        }
    }
}
void xyq::xhttp_server::manage_http_connect()
{
    /*
    @brief  对http连接进行管理
    */
    while (true)
    {

        try
        {
            for (auto &&kv : this->connects)
            {
                auto &&id = kv.first;
                auto &&con = kv.second;
                auto &&time_now = std::chrono::system_clock::now();
                if (time_now - con->start_time_point >= std::chrono::seconds(CONNECT_LIFE_TIME))
                {
                    std::cout << xyq::get_time_now() << "<" << con->__cid << ">"
                              << "线程超时，关闭连接->ID[" << id << "]\tIP " << con->__ip << std::endl;
                    std::thread th(&xyq::xhttp_connect::time_out, con);
                    th.detach();
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(CONNECT_SCAN_TIME));
        }
        catch (xconnect_exception e)
        {
            std::cout << e.what() << std::endl;
        }
        // std::cout << "存在连接数量：" << this->connects.size() << std::endl;
    }
}
void xyq::xhttp_server::run()
{
    // 检查服务器状态
    switch (this->__status)
    {
    case XSERVER_ERROR:
        throw xyq::xserver_exception("服务器发生异常，进行run操作！");
        break;
    case LISTENING:
        break;
    default:
        this->xs_listen();
    }
    if (this->__status == xyq::xserver_status::LISTENING)
    {
        std::cout << "服务器就绪，进行运行状态，地址--> ";
        std::cout << "http://" << this->__ip << ":" << this->__port << std::endl;
    }
    else
    {
        throw xyq::xserver_exception("服务器无法正常工作！");
    }
    // 服务器就绪，进入运行
    std::thread con_manager(&xhttp_server::manage_http_connect, this);
    con_manager.detach();
    this->accept_http_connect();
}

void xyq::xserver_base::show_info() const
{
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%s/%s:%d %d %d", this->__protocal.data(), this->__ip.data(), this->__port, this->__status, this->__socket);
    std::cout << buffer << std::endl;
}

inline void xyq::xserver_base::update_status(xserver_status xs_status)
{
    this->__last_status = this->__status;
    this->__status = xs_status;
}
inline void xyq::xconnect_base::update_status(xyq::xconnect_status status) noexcept
{
    this->__last_status = this->__status;
    this->__status = status;
}

xyq::xconnect_base::xconnect_base(int socket, const struct sockaddr_in &addr)
{
    this->__socket = socket;
    this->__addr = addr;
    this->__ip = inet_ntoa(addr.sin_addr);
    this->__port = addr.sin_port;
}
int xyq::xconnect_base::xc_write(const std::string &content)
{
    return this->xc_write(content.data());
}
int xyq::xconnect_base::xc_write(const char *content)
{
    if (not this->ready)
    {
        return 0;
        throw xyq::xconnect_exception("客户端已经断开连接，写入socket失败");
    }
    int write_len = write(this->__socket, content, strlen(content));
    if (write_len == -1)
    {
        this->ready = false;
        return -1;
    }
    else
    {
        return write_len;
    }
}
int xyq::xconnect_base::xc_read(size_t size)
{
    if (not this->ready)
    {
        throw xyq::xconnect_exception("客户端已经断开连接，读取socket失败");
        return -1;
    }
    int cnt = 0;
    int buffer_size = sizeof(this->__buffer);
    memset(this->__buffer, 0, sizeof(this->__buffer));
    char *dst = this->__buffer;
    while (cnt < size and cnt < buffer_size - 1)
    {
        int read_len = read(this->__socket, dst, 1);
        if (read_len == -1)
        {
            this->ready = false;
            return -1;
        }
        else if (read_len == 0)
        {
            break;
        }
        else
        {
            cnt++;
            dst++;
        }
    }
    *dst = '\0';
    return cnt;
}
void xyq::xconnect_base::xc_close()
{
    close(this->__socket);
}
void xyq::xhttp_connect::xc_close()
{
    if (this->__status == xyq::xconnect_status::CLOSE)
        return;
    close(this->__socket);
    this->update_status(xyq::xconnect_status::CLOSE);
    if (this->__server != NULL)
        this->__server->remove_connect(this->__cid);
}
int xyq::xconnect_base::get_line()
{
    /*
    @brief 从客户端连接的缓冲区读取一行内容
    */
    char ch;
    char *dst = this->__buffer;
    int buf_size = sizeof(this->__buffer);
    int cnt = 0;
    memset(this->__buffer, 0, buf_size);
    while (read(this->__socket, &ch, 1) > 0)
    {
        if (ch == '\r')
        {
            continue;
        }
        else if (ch == '\n')
        {
            break;
        }
        else
        {
            *dst = ch;
            cnt++;
            dst++;
            if (cnt >= buf_size)
            {
                return -1;
                throw xyq::xconnect_exception("读取长度超过buffer最大容量");
            }
        }
    }
    return cnt;
}
void xyq::xconnect_base::show_info() const
{
    char buffer[128];
    snprintf(buffer,sizeof(buffer),  "CLINET CONNECTION:%d %s", this->__socket, this->__ip.data());
    std::cout << buffer << std::endl;
}

std::string xyq::xconnect_base::get_content() const
{
    return this->__buffer;
}
void xyq::xhttp_request::set_header(std::string key, std::string value)
{
    if (key.size() == 0 and value.size() == 0)
        return;
    this->req_header[key] = value;
}
void xyq::xhttp_request::set_header(const std::pair<std::string, std::string> &kv)
{
    this->set_header(kv.first, kv.second);
}
std::string xyq::xhttp_request::get_header(std::string header_key)
{
    if (this->req_header.find(header_key) == this->req_header.end())
    {
        return "";
    }
    else
    {
        return this->req_header[header_key];
    }
}
void xyq::xhttp_request::ana_url()
{
    char rest[HTTP_URL_SIZE];
    char path[HTTP_URL_SIZE];
    char key[HTTP_URL_SIZE];
    char value[HTTP_URL_SIZE];
    if (xyq::divide_str_by_separator(this->url.data(), path, rest, "?", sizeof(path), sizeof(rest)) == 0)
    {
        this->path = path;
        while (xyq::divide_str_by_separator(rest, value, rest, "&", sizeof(value), sizeof(rest)) == 0)
        {
            if (xyq::divide_str_by_separator(value, key, value, "=", sizeof(key), sizeof(value)) == 0)
            {
                this->params[key] = value;
            }
        }
        if (xyq::divide_str_by_separator(value, key, value, "=", sizeof(key), sizeof(value)) == 0)
        {
            this->params[key] = value;
        }
    }
    else
    {
        this->path = this->url;
    }
}
bool xyq::xhttp_request::get_enable() const
{
    return this->enable;
}
std::string xyq::xhttp_request::get_method() const
{
    return this->method;
}
std::string xyq::xhttp_request::get_ip() const
{
    return this->ip;
}
std::string xyq::xhttp_request::get_version() const
{
    return this->http_version;
}
std::string xyq::xhttp_request::get_path()
{
    return this->path;
}
std::string xyq::xhttp_request::text()
{
    return this->req_content;
}
std::string xyq::xhttp_response::to_string()
{
    // 写响应行
    this->buffer = this->http_version + ' ' + std::to_string(this->status_code) + ' ' + this->message + "\r\n";
    // 计算响应体长度
    size_t content_length = this->rsp_content.size();
    this->set_header("content-length", std::to_string(content_length).data());
    // 写响应头
    for (auto &kv : this->rsp_header)
    {
        this->buffer += kv.first + ": " + kv.second + "\r\n";
    }
    // 写空行，响应头内容结束
    this->buffer += "\r\n";
    // 写响应体
    this->buffer += this->rsp_content;
    return this->buffer;
}
xyq::xhttp_response::xhttp_response()
{
    this->ok();
    this->set_header("Content-Type", "text/html;charset=UTF-8");
    this->set_header("Connection", "close");
    // this->set_header("Content-Encoding", "gzip");
}
void xyq::xhttp_response::not_found()
{
    this->message = "NOT_FOUND";
    this->status_code = 404;
}
void xyq::xhttp_response::ok()
{
    this->message = "OK";
    this->status_code = 200;
}
void xyq::xhttp_response::bad_request()
{
    this->message = "BAD_REQUEST";
    this->status_code = 400;
}
void xyq::xhttp_response::time_out()
{
    this->message = "TIME_OUT";
    this->status_code = 408;
}
void xyq::xhttp_response::set_status_code(std::string status_code)
{
    int code = std::stoi(status_code);
    this->status_code = code;
}
void xyq::xhttp_response::set_status_code(int status_code)
{
    this->status_code = status_code;
}
void xyq::xhttp_response::set_message(std::string message)
{
    this->message = message;
}
void xyq::xhttp_response::set_header(std::string key, std::string value)
{
    if (key.size() == 0 and value.size() == 0)
        return;
    this->rsp_header[key] = value;
}
void xyq::xhttp_response::operator=(const xyq::xhttp_response &rsp)
{
    this->status_code = rsp.status_code;
    this->message = rsp.message;
    this->rsp_content = rsp.rsp_content;
    this->rsp_header = rsp.rsp_header;
}
void xyq::xhttp_response::set_content(std::string content)
{
    this->rsp_content = content;
}
void xyq::xhttp_response::add_content(std::string content)
{
    this->rsp_content += content;
}
xyq::xhttp_connect::xhttp_connect(int socket, const struct sockaddr_in &addr, xhttp_server *server) : xconnect_base(socket, addr)
{
    this->start_time_point = std::chrono::system_clock::now();
    if (server != NULL)
        this->__server = server;
}
xyq::xhttp_connect::xhttp_connect(int socket, const struct sockaddr_in &addr, xhttp_server *server, uint64_t id) : xconnect_base(socket, addr)
{
    this->start_time_point = std::chrono::system_clock::now();
    if (server != NULL)
    {
        this->__server = server;
        this->set_cid(id);
        this->__server->add_connect(this);
    }
}
xyq::xhttp_connect::~xhttp_connect()
{
    // std::cout << __cid << "~" << std::endl;
}
xyq::xhttp_request xyq::xhttp_connect::get_http_request()
{
    /*
    @brief
        从socket中获取http请求，并解析成xhttp_request对象
    @return
        返回一个xhttp_request对象
    */
    xhttp_request clnt_req;
    // 解析请求头
    int line_length = 0;
    line_length = this->get_line();
    if (line_length <= 6)
    {
        // 请求行长度有问题
        clnt_req.enable = false;
        return clnt_req;
    }
    auto &&req_line = this->ana_req_line();
    if (std::get<1>(req_line).size() == 0)
    {
        clnt_req.enable = false;
        return clnt_req;
    }
    clnt_req.method = std::get<0>(req_line);
    clnt_req.url = std::get<1>(req_line);
    clnt_req.http_version = std::get<2>(req_line);
    clnt_req.ana_url();
    clnt_req.ip = this->__ip;
    while (this->get_line())
    {
        auto &&kv = this->ana_key_value();
        clnt_req.set_header(kv);
    }
    size_t content_length = 0;
    if (clnt_req.req_header.find("Content-Length") != clnt_req.req_header.end())
        content_length = std::stoi(clnt_req.req_header["Content-Length"]);
    else if (clnt_req.req_header.find("content-length") != clnt_req.req_header.end())
        content_length = std::stoi(clnt_req.req_header["content-length"]);
    if (content_length)
    {
        this->xc_read(content_length);
        clnt_req.req_content = this->__buffer;
    }
    return clnt_req;
}
std::tuple<std::string, std::string, std::string> xyq::xhttp_connect::ana_req_line()
{
    /*
    解析http请求行
    */
    std::tuple<std::string, std::string, std::string> req_line;
    // std::cout << "解析请求行-->" << this->__buffer << std::endl;
    char rest[8192], prefix[1024];
    // 解析http请求方法
    if (xyq::divide_str_by_separator(this->__buffer, prefix, rest, " ", sizeof(prefix), sizeof(rest)) == 0)
        std::get<0>(req_line) = prefix;
    else
    {
        return {"", "", ""};
        throw xyq::xconnect_exception("http请求方法解析失败");
    }

    // 解析url
    if (xyq::divide_str_by_separator(rest, prefix, rest, " ", sizeof(prefix), sizeof(rest)) == 0)
        std::get<1>(req_line) = prefix;
    else
    {
        return {"", "", ""};
        throw xyq::xconnect_exception("url解析失败");
    }

    // 剩余部分为Version
    std::get<2>(req_line) = rest;
    return req_line;
}
std::pair<std::string, std::string> xyq::xhttp_connect::ana_key_value()
{
    /*
    解析请求头中的行
    */
    std::pair<std::string, std::string> kv;
    char key[HTTP_REQUEST_LINE_SIZE];
    char value[HTTP_REQUEST_LINE_SIZE];
    if (xyq::divide_str_by_separator(this->__buffer, key, value, ": ", sizeof(key), sizeof(value)) == 0)
    {
        // 解析成功
        kv.first = key;
        kv.second = value;
        return kv;
    }
    else
    {
        // 解析失败
        return {"", ""};
    }
}
void xyq::xhttp_connect::show_info()
{
    std::cout << "HTTP_CLIENT: " << this->__socket << " " << this->__ip << std::endl;
}

void xyq::xhttp_connect::do_http__()
{
    /*
    @brief
        与客户端进行实际通信，调用mapping函数处理
    */
    try
    {
        xhttp_response clnt_rsp;
        // 解析http请求
        auto &&clnt_req = this->get_http_request();
        if (not clnt_req.enable)
        {
            clnt_rsp.bad_request();
            this->update_status(xyq::xconnect_status::FINISH);
            std::cout << xyq::get_time_now() << "<" << this->__cid << ">"
                      << " Get bad request!" << std::endl;
        }
        else
        {
            auto &&path = clnt_req.get_path();
            std::cout << xyq::get_time_now() << "<" << this->__cid << ">"
                      << "[" << clnt_req.get_ip() << "]: " << clnt_req.get_method() << " " << clnt_req.get_path() << std::endl;
            auto rsp_func = this->__server->get_path_mapping(path);
            if (rsp_func)
            {
                this->update_status(xyq::xconnect_status::RUNNING);
                clnt_rsp = rsp_func(clnt_req);
                if (this->__status == RUNNING)
                {
                    this->update_status(xyq::xconnect_status::FINISH);
                    std::cout << "FINISH--> " << this->__cid << std::endl;
                }
            }
            else
            {
                clnt_rsp.not_found();
                this->update_status(xyq::xconnect_status::FINISH);
            }

            std::cout << xyq::get_time_now() << "<" << this->__cid << ">"
                      << "Return--> " << clnt_rsp.status_code << " " << clnt_rsp.message << std::endl;
        }
        // 返回http响应
        if (this->__status == xyq::xconnect_status::FINISH)
            this->put_http_response(clnt_rsp);
        update_status(xyq::xconnect_status::RESPONSE_FINISH);
        // 关闭与客户端连接
        this->xc_close();
    }
    catch (xyq::xconnect_exception e)
    {
        std::cout << xyq::get_time_now() << " [ERROR] " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << xyq::get_time_now() << " [ERROR] " << std::endl;
    }
}
void xyq::xhttp_connect::put_http_response(xyq::xhttp_response &rsp)
{
    /*
    向客户端发送相应
    */
    auto &&rsp_str = rsp.to_string();
    this->xc_write(rsp_str);
}

xyq::xhttp_response xyq::render(std::string path)
{
    /*
    直接读取html文件
    */
    // using xyq::template_path;
    std::string fpath = template_path + path;
    xhttp_response ret;
    if (access(fpath.data(), 0) != 0)
    {
        // 未找到路径
        ret.not_found();
        return ret;
    }
    std::string buffer;
    std::fstream html_in;
    html_in.open(fpath, std::ios::in);
    while (std::getline(html_in, buffer))
    {
        ret.add_content(buffer + '\n');
    }
    return ret;
}
xyq::xhttp_response xyq::render(std::string path, std::unordered_map<std::string, std::string> &maps)
{
    /*
    渲染html文件
    */
    // using xyq::template_path;
    xhttp_render reader(path, maps);
    xyq::xhttp_response rsp;
    if (reader.enable())
    {
        reader.scan();
        rsp.add_content(reader.get_result());
    }
    else
    {
        rsp.not_found();
    }
    return rsp;
}
std::string xyq::xhttp_render::get_result()
{
    return this->result;
}
xyq::xhttp_render::xhttp_render(std::string path, std::unordered_map<std::string, std::string> &map)
{
    if (access((template_path + path).data(), 0) != 0)
    {
        this->path = "";
        return;
    }
    this->maps = map;
    this->path = template_path + path;
    reader.open(this->path, std::ios::in);
}
bool xyq::xhttp_render::enable()
{
    return this->path.size();
}
void xyq::xhttp_render::scan()
{
    /*
    扫描html文件进行处理
    */
    char ch;
    bool flag = false;
    while (not reader.eof())
    {
        ch = 0;
        reader.read(&ch, 1);
        switch (ch)
        {
        case '<':
            flag = true;
            break;
        case '[':
            if (flag)
            {
                trans();
                flag = false;
            }
            else
            {
                this->result += ch;
            }
            break;
        case 0:
            break;
        default:
            if (flag)
            {
                flag = false;
                this->result += '<';
            }
            this->result += ch;
            break;
        }
    }
    std::cout << this->result << std::endl;
}
void xyq::xhttp_render::trans()
{
    /*
    对html文件进行内容替换
    */
    char ch;
    bool flag = false;
    std::string key;
    while (not reader.eof())
    {
        ch = 0;
        reader.read(&ch, 1);
        if (ch == ']')
        {
            flag = true;
        }
        else if (ch == '>')
        {
            if (flag)
                break;
            else
                continue;
        }
        else if (ch == ' ')
        {
            continue;
        }
        else if (ch == '\n')
        {
            return;
        }
        else if (ch == 0)
        {
            continue;
        }
        else
        {
            key += ch;
        }
    }
    std::cout << key << std::endl;
    if (key.size() == 0)
        return;
    auto &&it = this->maps.find(key);
    if (it != this->maps.end())
    {
        this->result.append(maps[key]);
    }
}

uint64_t xyq::xhttp_server::get_connect_id() noexcept
{
    /*
    从服务器获取为连接分配的connect_id
    */
    auto id = this->__connect_count++;
    return id;
}
void xyq::xhttp_connect::set_cid(uint64_t id) noexcept
{
    this->__cid = id;
}
void xyq::xhttp_connect::set_pid(pthread_t id) noexcept
{
    this->__pid = id;
}
void xyq::xhttp_connect::time_out()
{
    /* 连接超时处理 */
    if (this->__status == xconnect_status::FINISH)
    {
        std::cout << "already closed" << std::endl;
        std::cout << this->__status << std::endl;
        return;
    }

    this->update_status(xconnect_status::TIME_OUT);
    pthread_cancel(this->__pid);
    xhttp_response rsp;
    rsp.time_out();
    this->put_http_response(rsp);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    this->xc_close();
}
void xyq::xhttp_server::add_connect(xhttp_connect *con)
{
    /* 向服务器连接管理列表中中加入某个连接 */
    auto id = con->__cid;
    std::cout << "add_path: " << id << std::endl;
    this->connects[id] = con;
}
int xyq::xhttp_server::remove_connect(uint64_t id)
{
    /* 根据id在服务器中删除连接 */
    auto &&it = this->connects.find(id);
    if (it != this->connects.end())
    {
        delete it->second;
        this->connects.erase(it);
        // std::cout << "remove_path: " << id << std::endl;
        return 0;
    }
    else
    {
        return -1;
    }
}