// 引入xServer头文件
#include "xServer.h"
#include <chrono>
// 定义服务器IP地址和端口
#define XHTTP_IP "127.0.0.1"
#define XHTTP_PORT 1120

// 定义业务处理逻辑
xyq::xhttp_response Myfunc(xyq::xhttp_request req) {
    xyq::xhttp_response clnt_rsp;
    for (auto &kv: req.params) {
        std::cout<<clnt_rsp.rsp_content<<std::endl;
        clnt_rsp.add_content(kv.first + " = " + kv.second + "\n");
    }
    std::cout<<clnt_rsp.rsp_content<<std::endl;
    return clnt_rsp;
}

xyq::xhttp_response test_html(xyq::xhttp_request req) {
    std::unordered_map<std::string, std::string> mp;
    mp["1"] = "123";
    mp["2"] = "4435";
    return xyq::render("xyq.html", mp);
}

int main() {
    // 初始化http服务器
    xyq::xhttp_server x_srv(XHTTP_IP, XHTTP_PORT);
    // 添加业务处理逻辑与url映射关系
    x_srv.add_path("/params", Myfunc);
    x_srv.add_path("/", test_html);
    // 运行服务器
    try {
        x_srv.run();
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }

    // 服务器运行完毕，回收服务器资源
    x_srv.xs_close();
}