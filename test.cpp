#include "xyq/xyq.h"
#include <iostream>
#include <optional>

using namespace std;

int main()
{

    // char s[] = "12345678";
    // char s1[] = "123";
    // const char* si = strstr(s, s1);
    // if (si == nullptr){
    //     cout<<"not find"<<endl;
    // }
    // else{
    //     cout<<"find it"<<endl;
    // }
    char sep[] = ": ";
    char line[] = "length: 123";
    char key[7];
    char value[4];
    auto ret = xyq::divide_str_by_separator(line, key, value, sep, sizeof(key), sizeof(value));

    cout << ret << endl;
    cout << key << endl;
    cout << value << endl;
}