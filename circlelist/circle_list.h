#ifndef DYB_CIRCLE_LIST
#define DYB_CIRCLE_LIST

#include <functional>

#include "debug.h"

namespace dyb
{
    struct node
    {
        int num;
        node * prev, * next;
        node(int n)
            : num(n), prev(nullptr), next(nullptr)
        {
        }
    };

    using std::function;


    // there are two different iterators in this container, "loop_iterator" and the common one "iterator"
    // the differences between them are how to treat the end() iterator
    // for the iterator :
    // the end() is actually a nullptr,
    // it's merely a tag used in the guard (iter != end()) to stop the loop 
    // after accessing the node immediately previous to head.
    // to achieve this, we also change iter to nullptr when it try to access the head for the second time
    // for the loop_iterator :
    // end() always equals to begin()
    // so loop_iterator can be used to iterate from tail to head FORWARD while iterator will stop at end()
    // function :
    // as a result, we have to apply two sets of algorithm function for this two different iterator.
    // the functions dealing with iterator work just the same as the STL function with a iter != end guard,
    // while the functions dealing with loop_iterator, for example do_something(_start, _terminal),
    // will not stop immediately after called when _start == _terminal,
    // but will stop when iter try to access the _start for the second time.
    // and of course, when _start != _terminal, common stl algorithm can be used perfectly for loop_iterator,
    // and the iter will not stop even it come cross end()

    // both are not implemented now and I use pointer instead
    struct circular_list
    {
        node * head = nullptr;
        int _size = 0;

        void insert(node * location, int num);
        void erase(node * location);
        // return nullptr when not found
        node * find(node * _begin, node * _end, function<bool(const node & n)> pred);
        size_t size() const { return _size; }
        node * begin() { return head; }
        node * end() { return nullptr; }
        node * loop_begin() { return head; }
        node * loop_end() { return head; }
    };

    void circular_list::insert(node * location, int num)
    {
        if (head == nullptr)
        {
            DEBUGCHECK(location == nullptr,
                "circular_list is empty but location is not nullptr");
            head = new node(num);
            head->next = head;
            head->prev = head;
            _size = 1;
        }
        else if (location != nullptr)
        {
            auto eq = [location](const node & n) { return &n == location; };
            DEBUGCHECK(find(head, head, eq) != nullptr, "location is not in the circular_list");
            node * left = location->prev;
            node * p = new node(num);
            left->next = p;
            p->prev = left;
            p->next = location;
            location->prev = p;
            ++_size;
            if (head == location)
                head = p;
        }
        else if (location == nullptr) // head != nullptr
        {
            node * left = head->prev;
            node * p = new node(num);
            left->next = p;
            p->prev = left;
            p->next = head;
            head->prev = p;
            ++_size;
        }
    }

    void circular_list::erase(node * location)
    {
        DEBUGCHECK(head != nullptr, "erase a node on a empty circular_list");
        auto eq = [location](const node & n) { return &n == location; };
        DEBUGCHECK(find(head, head, eq) != nullptr, "location is not in the circular_list");
        location->prev->next = location->next;
        location->next->prev = location->prev;
        --_size;
        if (_size == 0) head = nullptr;
        else if (head == location) head = head->next;
        delete location;
    }


    // work for loop_iterator
    node * circular_list::find(node * _begin, node * _end, function<bool(const node & n)> pred)
    {
        bool tag = _begin == _end;
        for (node * p = _begin; p != _end || tag; p = p->next)
        {
            if (pred(*p)) return p;
            tag = false;
        }
        return nullptr;
    }

}

#endif
