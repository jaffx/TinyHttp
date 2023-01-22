#include "xServer/xServer.h"
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;
class aa
{
public:
    void show(int a)
    {
        for (int i = 0; i < a; i++)
        {
            cout << i << endl;
        }
    }
};
// void show(int a)

int main()
{
    aa a;
    std::thread th(&aa::show, &a, 2);
    th.join();
}