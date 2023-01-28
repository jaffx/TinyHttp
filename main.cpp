// 引入xServer头文件
#include "xServer.h"
#include <chrono>
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
xyq::xhttp_response wait(xyq::xhttp_request req)
{
    xyq::xhttp_response clnt_rsp;
    std::cout << "wait start!!" << std::endl;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // std::cout << "waiting!!" << std::endl;
    }

    std::cout << "wait finish!!" << std::endl;
    return clnt_rsp;
}

int main()
{
    // 初始化http服务器
    xyq::xhttp_server x_srv(XHTTP_IP, XHTTP_PORT);
    // 添加业务处理逻辑与url映射关系
    x_srv.add_path("/params", Myfunc);
    x_srv.add_path("/", wait);
    // 运行服务器
    try
    {
        x_srv.run();
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }

    // 服务器运行完毕，回收服务器资源
    x_srv.xs_close();
}