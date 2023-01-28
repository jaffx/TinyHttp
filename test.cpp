#include <unordered_map>
#include <iostream>
using namespace std;

class A
{
public:
    void ddd()
    {
        cout << 123 << endl;
    }
};
int main()
{
    A *a = new A;
    delete a;
    a->ddd();
}