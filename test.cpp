#include "xyq/xyq.h"
#include <iostream>
#include <fstream>

using namespace std;

int main()
{
    char buffer[20];
    fstream hin;
    // hin.open("template/html/xyq.html", std::ios::in| std::ios::out);
    hin.open("testfile.txt", std::ios::in| std::ios::out);
    while(!hin.eof()){
        memset(buffer, 0, 5);
        hin.read(buffer, 20);
        cout<<strlen(buffer)<<buffer<<endl;
    }
    hin.close();
}