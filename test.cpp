#include <unordered_map>
#include <iostream>
#include <fstream>
#include "xServer.h"
using namespace std;

int main()
{
    auto ret = xyq::get_content_from_file("makefile");
    cout<<ret<<endl;
    
}