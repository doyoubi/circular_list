#include <iostream>
#include "circle_list.h"
#include "debug.h"

using std::cout;
using std::endl;
using dyb::debugCheck;
using dyb::circular_list;
using dyb::node;

int main()
{
    circular_list cl;
    cl.insert(cl.begin(), 0);
    cl.insert(cl.begin(), 1);
    cl.insert(cl.begin()->next, 2);
    node * r = cl.find(cl.loop_begin(), cl.loop_end(), [](const node & n){ return n.num == 2; });
    DEBUGCHECK(r->prev->num == 1, "error result");
    DEBUGCHECK(r->next->num == 0, "error result");
    cl.erase(cl.begin());
    cl.insert(nullptr, 3);
    cout <<"size: "<< cl.size() << endl;
    node * p = cl.head;
    do if (p != nullptr)
    {
        cout << p->num << endl;
        p = p->next;
    } while (p != cl.head);
    return 0;
}