#include <iostream>
#include <list>
#include "circle_list.h"
#include "debug.h"

using std::cout;
using std::endl;
using std::list;
using dyb::debugCheck;
using dyb::circular_list;

int main()
{
    circular_list<int> cl;
    cl.insert(cl.end(), 0);
    cl.insert(cl.end(), 1);
    cl.insert(cl.end(), 2);
    for (int n : cl) cout << n << endl;
    auto r = cl.find_if(cl.loop_begin(), cl.loop_end(), [](int n){ return n == 2; });
    cl.erase(cl.begin());
    cout <<"size: "<< cl.size() << endl;
    for (int n : cl) cout << n << endl;
    return 0;
}
