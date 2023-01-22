#include "xServer.h"

std::string xyq::template_path = "template/";
xyq::xserver_base::xserver_base(std::string protocal, std::string ip, __uint16_t port)
{
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
    switch (this->__status)
    {
    case (SOCK_READY):
        // socket 就绪，可以进行bind操作
        if (bind(this->__socket, (struct sockaddr *)(&this->__addr), sizeof(this->__addr)) == -1)
        {
            // bind失败
            this->update_status(XSERVER_ERROR);
            xyq::xserver_exception("bind失败，该端口已被占用");
            return -1;
        }
        else
        {
            // bind成功
            this->update_status(BIND_READY);
            return 1;
        }
        break;
    case (XSERVER_ERROR):
        throw xyq::xserver_exception("服务器发生错误，无法进行BIND操作");
        break;
    default:
        break;
    }
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
xyq::xconnect_base xyq::xserver_base::xs_get_connect()
{
    if (this->__status != LISTENING)
        this->xs_listen();
    sockaddr_in clnt_addr;
    socklen_t clntaddr_len = sizeof(clnt_addr);
    int clnt_sock = accept(this->__socket, (struct sockaddr *)(&clnt_addr), &clntaddr_len);
    xconnect_base clnt_con(clnt_sock, clnt_addr);
    return clnt_con;
}
xyq::xhttp_connect xyq::xhttp_server::xs_get_connect()
{
    // 如果没有进行listen，则进行listen操作
    if (this->__status != LISTENING)
        this->xs_listen();

    sockaddr_in clnt_addr;
    socklen_t clntaddr_len = sizeof(clnt_addr);
    int clnt_sock = accept(this->__socket, (struct sockaddr *)(&clnt_addr), &clntaddr_len);
    xhttp_connect clnt_con(clnt_sock, clnt_addr, this);
    return clnt_con;
}
void xyq::xhttp_server::do_connect(xyq::xhttp_connect con)
{
    con.do_http__();
    // con.xc_close();
}
void xyq::xhttp_server::add_path(std::string path, xyq::xhttp_rsp_func rsp_function)
{
    if (path.size() == 0)
        return;
    if (this->rsp_mapping.find(path) != this->rsp_mapping.end())
        return;
    this->rsp_mapping[path] = rsp_function;
}
xyq::xhttp_rsp_func xyq::xhttp_server::get_path_mapping(std::string url)
{
    if (this->rsp_mapping.find(url) == this->rsp_mapping.end())
        return NULL;
    else
        return this->rsp_mapping[url];
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
    // 服务器就绪，进入运行
    std::cout << "服务器就绪，进行运行状态" << std::endl;
    while (true)
    {
        // 等待服务器连接请求
        auto &&clnt_con = this->xs_get_connect();
        std::cout << clnt_con.__ip << std::endl;
        std::thread *th = new std::thread(&xhttp_server::do_connect, this, clnt_con);
        th->detach();
    }
}

void xyq::xserver_base::show_info() const
{
    char buffer[128];
    sprintf(buffer, "%s/%s:%d %d %d", this->__protocal.data(), this->__ip.data(), this->__port, this->__status, this->__socket);
    std::cout << buffer << std::endl;
}

inline void xyq::xserver_base::update_status(xserver_status xs_status)
{
    this->__last_status = this->__status;
    this->__status = xs_status;
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
    this->xc_write(content.data());
}
int xyq::xconnect_base::xc_write(const char *content)
{
    if (not this->ready)
    {
        throw xyq::xserver_exception("客户端已经断开连接，写入socket失败");
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
        throw xyq::xserver_exception("客户端已经断开连接，读取socket失败");
        return -1;
    }
    int cnt = 0;
    int buffer_size = sizeof(this->__buffer);
    memset(this->__buffer, 0, sizeof(this->__buffer));
    char *dst = this->__buffer;
    while (cnt < size and cnt < buffer_size - 1)
    {
        int read_len = read(this->__socket, dst, 1);
        // std::cout << *dst << std::endl;
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
int xyq::xconnect_base::get_line()
{
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
                throw xyq::xserver_exception("读取长度超过buffer最大容量");
        }
    }
    return cnt;
}
void xyq::xconnect_base::show_info() const
{
    char buffer[128];
    sprintf(buffer, "CLINET CONNECTION:%d %s", this->__socket, this->__ip.data());
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
    // std::cout<<this->buffer<<std::endl;
    return this->buffer;
}
xyq::xhttp_response::xhttp_response()
{
    this->ok();
    this->set_header("Content-Type", "text/html;charset=UTF-8");
    // this->set_header("Content-Encoding", "gzip");
}
void xyq::xhttp_response::not_found()
{
    this->message = "Not_found";
    this->status_code = 404;
}
void xyq::xhttp_response::ok()
{
    this->message = "OK";
    this->status_code = 200;
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
    this->get_line();
    auto &&req_line = this->ana_req_line();
    clnt_req.method = std::get<0>(req_line);
    clnt_req.url = std::get<1>(req_line);
    clnt_req.http_version = std::get<2>(req_line);
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
        // std::cout<<this->__buffer<<std::endl;
        clnt_req.req_content = this->__buffer;
    }

    return clnt_req;
}
std::tuple<std::string, std::string, std::string> xyq::xhttp_connect::ana_req_line()
{
    std::tuple<std::string, std::string, std::string> req_line;
    char rest[8192], prefix[1024];
    // 解析http请求方法
    if (xyq::divide_str_by_separator(this->__buffer, prefix, rest, " ", sizeof(prefix), sizeof(rest)) == 0)
        std::get<0>(req_line) = prefix;
    else
        throw xyq::xserver_exception("http请求方法解析失败");
    // 解析url
    if (xyq::divide_str_by_separator(rest, prefix, rest, " ", sizeof(prefix), sizeof(rest)) == 0)
        std::get<1>(req_line) = prefix;
    else
        throw xyq::xserver_exception("url解析失败");
    // 剩余部分为Version
    std::get<2>(req_line) = rest;
    // std::cout << std::get<0>(req_line) << std::endl;
    // std::cout << std::get<1>(req_line) << std::endl;
    // std::cout << std::get<2>(req_line) << std::endl;
    return req_line;
}
std::pair<std::string, std::string> xyq::xhttp_connect::ana_key_value()
{
    std::pair<std::string, std::string> kv;
    char key[HTTP_REQUEST_LINE_SIZE];
    char value[HTTP_REQUEST_LINE_SIZE];
    if (xyq::divide_str_by_separator(this->__buffer, key, value, ": ", sizeof(key), sizeof(value)) == 0)
    {
        // 解析成功
        kv.first = key;
        kv.second = value;
        // std::cout << key << ": " << value << std::endl;
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

    // 解析http请求
    auto &&clnt_req = this->get_http_request();
    // 处理path信息
    auto &&path = clnt_req.url;
    std::cout << clnt_req.method << "\t" << clnt_req.url << "\t" << clnt_req.http_version << std::endl;
    xhttp_response clnt_rsp;
    auto rsp_func = this->__server->get_path_mapping(path);
    if (rsp_func)
    {
        clnt_rsp = rsp_func(clnt_req);
    }
    else
        clnt_rsp.not_found();
    // 返回http响应
    this->do_http_response(clnt_rsp);
    // 打个日志记录一下
    std::cout << "[Get URL]\t" << path << std::endl;
    // 关闭与客户端连接
    this->xc_close();
}
void xyq::xhttp_connect::do_http_response(xyq::xhttp_response &rsp)
{
    auto &&rsp_str = rsp.to_string();
    // std::cout << "######################" << std::endl;
    // std::cout << rsp_str << std::endl;
    // std::cout << "######################" << std::endl;
    this->xc_write(rsp_str);
}

xyq::xhttp_response xyq::render(std::string path)
{
    // using xyq::template_path;
    std::string fpath;
    xhttp_response ret;
    if (access(path.data(), 0) == 0)
    {
        // path直接指出路径
        fpath = path;
    }
    else if (access((template_path + path).data(), 2))
    {
        // 通过template和path计算出路径
        fpath = template_path + path;
    }
    else
    {
        // 未找到路径
        std::cout << path << "文件不存在" << std::endl;
        ret.not_found();
        return ret;
    }
    // if (access(fpath.data(), 4) == 0)
    // {
    //     // 文件存在但是不可读
    //     std::cout << "文件不可读" << std::endl;
    //     ret.not_found();
    //     return ret;
    // }
    std::string buffer;
    std::fstream html_in;
    html_in.open(fpath, std::ios::in);
    while (std::getline(html_in, buffer))
    {
        ret.add_content(buffer + '\n');
    }
    return ret;
}