#include "xhttp.h"

void do_http_request(int sock)
{
    struct sockaddr_in clnt_addr;
    socklen_t clnt_sock_len = sizeof(clnt_addr);
    char linebuf[2048];
    char kbuf[1024], vbuf[1024];
    int line_count = 0;
    int line_len = 0;
    char content[2048];
    req_line clnt_req_line;

    std::optional<kv_pair> kvret;
    kv_pair kvp;

    std::unordered_map<string, string> req_header;
    // 获取请求行并解析
    line_len = get_line(sock, linebuf, sizeof(linebuf));
    if (line_len <= 0)
    {
        cout << "读取请求行失败！" << endl;
    }
    auto clnt_req_line_opt = get_http_request_line(linebuf);
    if (clnt_req_line_opt == std::nullopt)
    {
        cout << "请求行解析失败！" << endl;
    }
    else
    {
        clnt_req_line = clnt_req_line_opt.value();
        show_xhttp_req_line(clnt_req_line_opt.value());
    }

    // 处理http请求头
    while (line_len = get_line(sock, linebuf, sizeof(linebuf)) > 0)
    {
        // cout << "[" << line_count++ << "]" << strlen(linebuf) << "" << linebuf << endl;
        kvret = get_key_value(linebuf);

        if (kvret == std::nullopt)
        {
            cout << "请求行解析失败" << endl;
        }
        else
        {
            kvp = kvret.value();
            req_header[kvp.first] = kvp.second;
        }
    }
    if (line_len == -1)
    {
        cerr << "行读取失败" << endl;
        return;
    }
    // for(const kv_pair & kv : req_header){
    //     cout<<kv.first<<": "<<kv.second<<endl;
    // }
    int content_length = 0;
    if (req_header.find("content-length") == req_header.end())
    {
        content_length = 0;
    }
    else
    {
        content_length = stoi(req_header["content-length"]);
    }
    auto && url_info_opt = ana_url(clnt_req_line.url);
    struct xhttp_url_info clnt_url_info ;
    if(url_info_opt==std::nullopt){
        cout<<"URL解析失败"<<endl;
    }
    else{
        clnt_url_info = url_info_opt.value();
    }
    cout<<clnt_url_info.path<<endl;
    for(auto & kv : clnt_url_info.params){
        cout<<kv.first<<"="<<kv.second<<endl;
    }
    not_found(sock);
    close(sock);
}

int not_found(int sock)
{
    char write_buffer[2048] = "HTTP/1.1 404 Not Found\r\n\r\n";
    write(sock, write_buffer, strlen(write_buffer));
    return 0;
}

std::optional<req_line> get_http_request_line(const char *line)
{
    int idx = 0;
    int cnt = 0;
    char ch;
    int len = strlen(line);
    req_line rl;
    // 获取method
    while ((ch = line[idx++]) != ' ' and idx < len)
    {
        if (cnt < sizeof(rl.method))
            rl.method[cnt++] = ch;
    }
    if (idx >= len)
        return std::nullopt;

    rl.method[cnt] = '\0';
    // 获取url
    cnt = 0;
    while ((ch = line[idx++]) != ' ' and idx < len)
    {
        if (cnt < sizeof(rl.url))
            rl.url[cnt++] = ch;
    }
    if (idx >= len)
        return std::nullopt;
    rl.url[cnt] = '\0';
    // 获取version
    cnt = 0;
    while (idx < len)
    {
        if (cnt < sizeof(rl.version))
            rl.version[cnt++] = line[idx];
        idx++;
    }
    rl.version[cnt] = '\0';
    return rl;
}

void show_xhttp_req_line(req_line &req)
{
    /*
    展示请求行信息
    */

    cout << "Method :" << req.method << endl;
    cout << "URL    :" << req.url << endl;
    cout << "VERSION:" << req.version << endl;
}

void show_sockaddr_in(const struct sockaddr_in &addr)
{
    /*
    展示sock地址信息，sockaddr_in格式
    */
    cout << "Address: " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << endl;
}

int get_line(int sock, char *buf, int size)
{
    /*
    从缓冲区读取一行数据，遇到/r/n结束
    */
    int count = 0, len = 0;
    char ch = '\0';
    while (count < size and ch != '\n')
    {
        len = read(sock, &ch, 1);
        /*
        这里存在一个疑问，如果http请求中存在一个\r符号怎么处理？
        */
        if (len > 0)
        {
            if (ch == '\r')
                continue;
            else if (ch == '\n')
            {
                break;
            }
            buf[count++] = ch;
        }
        else if (len == -1)
        {
            count = -1;
            cerr << "读取内容失败" << endl;
            break;
        }
        else
        {
            count = -1;
            cerr << "客户端关闭连接" << endl;
            break;
        }
    }
    if (count >= 0)

        buf[count] = '\0';

    return count;
}

std::optional<kv_pair> get_key_value(const char *buf)
{
    /*
    从请求头中的一行获取key和value
    */
    char key[1024];
    char value[1024];
    if (xyq::divide_str_by_separator(buf, key, value, ": ", sizeof(key), sizeof(value)) == 0)
        return make_pair(string(key), string(value));
    else
        return std::nullopt;
}

int read_html_file(const char *filepath, char *buffer)
{
    std::ifstream html;
    html.open(filepath);
    html >> buffer;
    html.close();
}

std::optional<struct xhttp_url_info> ana_url(const char *url)
{
    /*
    解析url格式，并分析其中参数
    */
    struct xhttp_url_info url_info;
    char path[URL_PATH_LEN], pkey[URL_PARAM_KLEN], pvalue[URL_PARAM_VLEN], rest[URL_MAX_LEN], buffer[URL_MAX_LEN], kv[URL_PARAM_KLEN + URL_PARAM_VLEN];
    if (xyq::divide_str_by_separator(url, path, rest, "?", sizeof(path), sizeof(rest)) == -1)
        // 没找到问号，说明没有参数
        goto RETURN;
    // 开始处理参数
    strcpy(buffer, rest);
    while (xyq::divide_str_by_separator(buffer, kv, rest, "&", sizeof(kv), sizeof(rest)) == 0)
    {
        // 不符合key=value格式的参数直接舍弃
        if (xyq::divide_str_by_separator(kv, pkey, pvalue, "=", sizeof(pkey), sizeof(pvalue)) == 0)
        {
            url_info.params[pkey] = pvalue;
            // std::cout << pkey << " := " << pvalue << std::endl;
        }
        strcpy(buffer, rest);
        // cout<<rest<<endl;
    }
    // 处理剩下的参数
    if (xyq::divide_str_by_separator(buffer, pkey, pvalue, "=", sizeof(pkey), sizeof(pvalue)) == 0)
    {
        url_info.params[pkey] = pvalue;
        // std::cout << "KEY_VALUE___  " << pkey << " := " << pvalue << std::endl;
    }
RETURN:
    cout << "PATH: " << path << endl;
    url_info.path=path;
    return url_info;
}
