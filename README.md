# xServer

*@author 徐永麒*

## 一、基本介绍

- xServer是一个轻量化http框架，提供了基本http请求和业务处理功能。
- 本框架由C++编写，基于socket实现http通信和异步业务处理。
- 本框架为作者练习项目，框架稳定性不能保证。

## 二、快速开始
1. 创建main.cpp文件
```C++
// 引入xServer头文件
#include "xServer.h"
// 定义服务器IP地址和端口
#define XHTTP_IP "127.0.0.1"
#define XHTTP_PORT 1120
// 定义业务处理逻辑
xyq::xhttp_response Myfunc(xyq::xhttp_request req)
{
    auto && ret = xyq::render("hello.html"); // 渲染hello.html文件
    return ret;
}
int main()
{
    xyq::xhttp_server x_srv(XHTTP_IP, XHTTP_PORT);// 初始化http服务器
    x_srv.add_path("/", Myfunc);// 添加url与业务处理逻辑的映射关系
    x_srv.run();// 运行服务器
    x_srv.xs_close();// 服务器运行完毕，回收服务器资源
}
```
2. 创建hello.html文件
```html
<!DOCTYPE html>
<html>
    <body style="background:red;">
        <h1>Hello world!</h1>
    </body>
</html>
```
3. 编译+运行，访问127.0.0.1:1120
## 三、使用说明
### 3.1 xhttp_request
1. 说明
    http请求类，封装了http请求内容，包括**请求方法、http版本号、请求url、请求头内容、请求体**等内容，由xconnect对象解析生成。
### 3.2 xhttp_response
1. 说明
    http响应类，封装http响应内容，包括**http版本、响应状态码、状态码信息、响应头、响应体**等内容，可以经过xconnect对象返回至客户端。
### 3.3 xhttp_connect
1. 说明
    http连接类，用以描述客户端连接，一般由服务器生成及管理，可以解析客户端内容生成xhttp_request对象，并将xhttp_connect对象解析成字符串并返回客户端。
### 3.4 xhttp_server
1. 继承自xserver_base类
   - xserver_base类
        xserver_base类提供了基本的socket操作，xserver_base方法会自动调用这些方法，这些方法对框架使用者来说透明。
2. 基本使用步骤
   1. 创建xhttp_server
        `xyq::xhttp_server xsrv("127.0.0.1",80);`
        需要指定IP地址和端口
    2. 添加业务处理逻辑映射
        - 业务处理函数模板
            `xyq::xhttp_response my_function(xyq::xhttp_request);`
        - 添加到服务器
            ` xsrv.add_path("mypath",my_function );`
        - 如果需要返回http文件，可以调用render函数
            `xyq::xhttp_response render(std::string file_path)`
    3. 启动服务器
        `xsrv.run();`
    4. 关闭服务器并回收服务器资源
        `xsrv.xs_close();`