#define _SCL_SECURE_NO_WARNINGS 1
#include <iostream>
#include <list>
#include <iterator>
#include "circle_list.h"
#include "debug.h"

using std::cout;
using std::endl;
using std::list;
using std::equal;
using std::begin;
using std::end;
using dyb::debugCheck;
using dyb::circular_list;

#define TEST(expression) DEBUGCHECK(expression, "test failed")

// core function
void test_exist()
{
    cout << "test_exist" << endl;
    using std::equal;
    circular_list<int> cl;
    for (int i = 0; i < 3; i++) cl.insert(end(cl), i);
    TEST(cl.size() == 3);
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 2 })));

    TEST(cl.exist(begin(cl)));
    TEST(!cl.exist(end(cl)));
    TEST(cl.exist(++begin(cl)));
    TEST(cl.exist(cl.loop_begin()));
    TEST(cl.exist(cl.loop_end()));
    TEST(cl.exist(++cl.loop_begin()));
    TEST(cl.exist(++cl.loop_end()));
    TEST(!cl.exist(circular_list<int>::common_iter(nullptr)));
    TEST(!cl.exist(circular_list<int>::common_iter(nullptr, nullptr)));
    TEST(!cl.exist(circular_list<int>::loop_iter(nullptr)));
}

void test_find_if()
{
    cout << "test_find_if" << endl;
    using std::equal;
    circular_list<int> cl;
    for (int i = 0; i < 3; i++) cl.insert(end(cl), i);
    TEST(cl.size() == 3);
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 2 })));

    for (int i = 0; i < 3; i++)
    {
        auto eq = [i](int n) { return n == i; };
        TEST(*cl.find_if(begin(cl), end(cl), eq) == i);
        TEST(*cl.find_if(cl.loop_begin(), cl.loop_end(), eq) == i);
    }
    // common iterator
    TEST(cl.find_if(begin(cl), end(cl), [](int n){ return n > 0; }) == ++begin(cl));
    TEST(*cl.find_if(begin(cl), end(cl), [](int n){ return n > 0; }) == 1);
    TEST(cl.find_if(begin(cl), end(cl), [](int n){ return n >= 0; }) == begin(cl));
    TEST(*cl.find_if(begin(cl), end(cl), [](int n){ return n >= 0; }) == 0);
    TEST(cl.find_if(begin(cl), end(cl), [](int n){ return n > 1; }) == ++(++begin(cl)));
    TEST(*cl.find_if(begin(cl), end(cl), [](int n){ return n > 1; }) == 2);
    TEST(cl.find_if(begin(cl), end(cl), [](int n){ return n < 0; }) == end(cl));
    // loop iterator
    TEST(cl.find_if(cl.loop_begin(), cl.loop_end(), [](int n){ return n > 0; }) == ++cl.loop_begin());
    TEST(*cl.find_if(cl.loop_begin(), cl.loop_end(), [](int n){ return n > 0; }) == 1);
    TEST(cl.find_if(cl.loop_begin(), cl.loop_end(), [](int n){ return n >= 0; }) == cl.loop_begin());
    TEST(*cl.find_if(cl.loop_begin(), cl.loop_end(), [](int n){ return n >= 0; }) == 0);
    TEST(cl.find_if(cl.loop_begin(), cl.loop_end(), [](int n){ return n > 1; }) == ++(++cl.loop_begin()));
    TEST(*cl.find_if(cl.loop_begin(), cl.loop_end(), [](int n){ return n > 1; }) == 2);
    TEST(cl.find_if(cl.loop_begin(), cl.loop_end(), [](int n){ return n < 0; }) == end(cl));
}

void test_insert()
{
    cout << "test_insert" << endl;
    using std::equal;
    circular_list<int> cl;
    TEST(cl.size() == 0);
    cl.insert(begin(cl), 1);
    TEST(cl.size() == 1);
    TEST(*begin(cl) == 1);

    cl.insert(++begin(cl), 2);
    TEST(cl.size() == 2);
    TEST(*(++begin(cl)) == 2);
    TEST(equal(begin(cl), end(cl), begin({ 1, 2 }) ));

    cl.insert(begin(cl), 0);
    TEST(cl.size() == 3);
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 2 })));

    cl.insert(end(cl), 3);
    TEST(cl.size() == 4);
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 2, 3 })));
}

void test_insert_loop_iter()
{
    cout << "test_insert_loop_iter" << endl;
    using std::equal;
    circular_list<int> cl;
    TEST(cl.size() == 0);
    cl.insert(cl.loop_begin(), 1);
    TEST(cl.size() == 1);
    TEST(*cl.loop_begin() == 1);

    cl.insert(++(cl.loop_begin()), 2);
    TEST(cl.size() == 2);
    TEST(*(cl.loop_begin()) == 2);
    TEST(equal(begin(cl), end(cl), begin({ 2, 1 })));

    cl.insert(cl.loop_begin(), 0);
    TEST(cl.size() == 3);
    TEST(equal(begin(cl), end(cl), begin({ 0, 2, 1 })));

    cl.insert(cl.loop_end(), 3);
    TEST(cl.size() == 4);
    TEST(equal(begin(cl), end(cl), begin({ 3, 0, 2, 1 })));
}

void test_erase()
{
    cout << "test_erase" << endl;
    using std::equal;
    circular_list<int> cl;
    for (int i = 0; i < 3; i++) cl.insert(end(cl), i);
    TEST(cl.size() == 3);
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 2 })));

    cl.erase(++(++begin(cl)));
    TEST(cl.size() == 2);
    TEST(equal(begin(cl), end(cl), begin({ 0, 1 })));

    cl.erase(begin(cl));
    TEST(cl.size() == 1);
    TEST(equal(begin(cl), end(cl), begin({ 1 })));

    cl.erase(begin(cl));
    TEST(cl.size() == 0);
}

void test_erase_loop_iter()
{
    cout << "test_erase_loop_iter" << endl;
    using std::equal;
    circular_list<int> cl;
    for (int i = 0; i < 4; i++) cl.insert(end(cl), i);
    TEST(cl.size() == 4);
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 2, 3 })));

    cl.erase(++(++cl.loop_begin()));
    TEST(cl.size() == 3);
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 3 })));

    cl.erase(cl.loop_end());
    TEST(cl.size() == 2);
    TEST(equal(begin(cl), end(cl), begin({ 1, 3 })));

    cl.erase(cl.loop_begin());
    TEST(cl.size() == 1);
    TEST(equal(begin(cl), end(cl), begin({ 3 })));

    cl.erase(cl.loop_begin());
    TEST(cl.size() == 0);
}

// helper function
void test_constructor_operator()
{
    cout << "test_constructor_operator" << endl;
    circular_list<int> cl = { 0, 1, 2 };
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 2 })));

    // copy constructor
    circular_list<int> a = cl;
    TEST(equal(begin(a), end(a), begin({ 0, 1, 2 })));
    TEST(a.size() == 3);
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 2 })));
    TEST(cl.size() == 3);

    // move constructor
    a.insert(end(a), 3);
    circular_list<int> b = std::move(a);
    TEST(equal(begin(b), end(b), begin({ 0, 1, 2, 3 })));
    TEST(b.size() == 4);
    TEST(equal(begin(a), end(a), begin(list<int>{})));
    TEST(a.size() == 0);

    // operator =
    b.insert(begin(b), 9);
    TEST(equal(begin(b), end(b), begin({ 9, 0, 1, 2, 3 })));
    a = b;
    TEST(equal(begin(b), end(b), begin({ 9, 0, 1, 2, 3 })));
    TEST(b.size() == 5);
    TEST(equal(begin(a), end(a), begin({ 9, 0, 1, 2, 3 })));
    TEST(a.size() == 5);

    // move operator =
    a = std::move(cl);
    TEST(equal(begin(a), end(a), begin({ 0, 1, 2 })));
    TEST(a.size() == 3);
    TEST(equal(begin(cl), end(cl), begin(list<int>{})));
    TEST(cl.size() == 0);
}

void test_adjacent_find()
{
    cout << "test_adjacent_find" << endl;
    circular_list<int> cl;
    for (int i = 0; i < 3; i++)
        cl.insert(end(cl), i);
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 2 })));

    auto lt = [](int first, int next) { return first > next; };
    TEST(*dyb::adjacent_find(cl.loop_begin(), cl.loop_end(), lt) == 2);
    TEST(dyb::adjacent_find(cl.loop_begin(), cl.loop_end(), lt) == ++(++cl.loop_begin()));
    auto f = [](int first, int next) { return first == 1 && next == 2; };
    TEST(*dyb::adjacent_find(cl.loop_begin(), cl.loop_end(), f) == 1);
    TEST(dyb::adjacent_find(cl.loop_begin(), cl.loop_end(), f) == ++cl.loop_begin());
}

void test_for_each()
{
    cout << "test_for_each" << endl;
    circular_list<int> cl;
    for (int i = 0; i < 4; i++)
    {
        cl.insert(end(cl), i);
    }
    TEST(equal(begin(cl), end(cl), begin({0, 1, 2, 3})));
    for_each(cl.loop_begin(), cl.loop_end(), [](int & n){ ++n; });
    TEST(equal(begin(cl), end(cl), begin({ 1, 2, 3, 4 })));
}

void test_common_iter_const()
{
    cout << "test_common_iter_const" << endl;
    struct obj
    {
        int func() { return 0; }
        int func() const { return 1; }
    };
    const circular_list<obj> c = { obj(), obj(), obj() };
    circular_list<obj> nc = { obj(), obj(), obj() };
    TEST(begin(nc)->func() == 0);
    for (auto & o : nc)
        TEST(o.func() == 0);
    for (auto o : nc)
        TEST(o.func() == 0);

    TEST(begin(c)->func() == 1);
    for (auto & o : c)
        TEST(o.func() == 1);
    for (auto o : c)
        TEST(o.func() == 0);
}

void test_loop_iter_const()
{
    cout << "test_loop_iter_const" << endl;
    struct obj
    {
        int func() { return 0; }
        int func() const { return 1; }
    };
    const circular_list<obj> c = { obj(), obj(), obj() };
    circular_list<obj> nc = { obj(), obj(), obj() };
    TEST(nc.loop_begin()->func() == 0);
    TEST(nc.loop_end()->func() == 0);

    TEST(c.loop_begin()->func() == 1);
    TEST(c.loop_end()->func() == 1);
}

int main()
{
    // core function
    test_exist();
    test_find_if();
    test_insert();
    test_insert_loop_iter();
    test_erase();
    test_erase_loop_iter();
    test_constructor_operator();
    test_common_iter_const();
    test_loop_iter_const();
    
    // helper function
    test_adjacent_find();
    test_for_each();

    cout << "all tests passed" << endl;
    return 0;
}
