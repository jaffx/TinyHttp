// 引入xServer头文件
#include "xServer.h"
#include <chrono>
#include "json/json.h"
// 定义服务器IP地址和端口
#define XHTTP_IP "127.0.0.1"
#define XHTTP_PORT 1120
// 定义业务处理逻辑
using namespace std;
xyq::xhttp_response test_time_out(xyq::xhttp_request req)
{
    xyq::xhttp_response rsp;
    while(1){
        cout<<"blocking"<<endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    rsp.not_found();
    return rsp;

}
xyq::xhttp_response test_message(xyq::xhttp_request req)
{
    xyq::xhttp_response rsp;
    auto t = chrono::system_clock::now();
    auto tt = chrono::system_clock::to_time_t(t);
    Json::Value v;
    v["status"] = 200;
    v["message"] = "success";
    v["data"] = {};
    v["data"]["time"] = int(tt);
    rsp.set_content(v.toStyledString());
    return rsp;
}
xyq::xhttp_response test_html(xyq::xhttp_request req)
{
    std::unordered_map<std::string, std::string> mp;
    mp["hello_name"] = "xuyongqi";
    return xyq::render("xyq.html", mp);
}
int main()
{
    // 初始化http服务器
    xyq::xhttp_server x_srv(XHTTP_IP, XHTTP_PORT);
    // 添加业务处理逻辑与url映射关系
    x_srv.add_path("/time_out", test_time_out);
    x_srv.add_path("/message", test_message);
    x_srv.add_path("/", test_html);
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