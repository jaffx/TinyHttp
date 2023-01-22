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
    // 渲染hello.html文件
    return xyq::render("hello.html");
}
int main()
{
    // 初始化http服务器
    xyq::xhttp_server x_srv(XHTTP_IP, XHTTP_PORT);
    // 添加业务处理逻辑与url映射关系
    x_srv.add_path("/", Myfunc);
    // 运行服务器
    x_srv.run();
    // 服务器运行完毕，回收服务器资源
    x_srv.xs_close();
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
3. 编译并运行，得到结果
   