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
void test_constructor()
{
    circular_list<int> cl = { 0, 1, 2 };
    TEST(equal(begin(cl), end(cl), begin({ 0, 1, 2 })));
}

void test_adjacent_find()
{
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

int main()
{
    // core function
    test_exist();
    test_find_if();
    test_insert();
    test_insert_loop_iter();
    test_erase();
    test_erase_loop_iter();

    // helper function
    test_constructor();
    test_adjacent_find();

    cout << "all tests passed" << endl;
    return 0;
}
