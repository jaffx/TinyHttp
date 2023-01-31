#include <unordered_map>
#include <iostream>
#include <fstream>
#include "xServer.h"
using namespace std;

int main()
{
    fstream rds("template/xyq.html");
    char ch;
    while (!rds.eof())
    {
        ch = 0;
        rds.read(&ch, 1);
        if (ch != 0)
            std::cout << ch;
    }
    // std::unordered_map<std::string, std::string> mp;
    // mp["1"] = "123";
    // mp["2"] = "4435";
    // xyq::xhttp_render rd("xyq.html", mp);
    // rd.scan();
}