// 引入xServer头文件
#include "xServer.h"
// 定义服务器IP地址和端口
#define XHTTP_IP "127.0.0.1"
#define XHTTP_PORT 1120
// 定义业务处理逻辑
xyq::xhttp_response Myfunc(xyq::xhttp_request req)
{
    xyq::xhttp_response clnt_rsp;
    for (auto &kv : req.params)
    {
        clnt_rsp.add_content(kv.first + " = " + kv.second + "\n");
    }
    return clnt_rsp;
}
int main()
{
    // 初始化http服务器
    xyq::xhttp_server x_srv(XHTTP_IP, XHTTP_PORT);
    // 添加业务处理逻辑与url映射关系
    x_srv.add_path("/params", Myfunc);
    // 运行服务器
    x_srv.run();
    // 服务器运行完毕，回收服务器资源
    x_srv.xs_close();
}