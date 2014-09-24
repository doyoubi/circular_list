#ifndef DYB_CIRCLE_LIST
#define DYB_CIRCLE_LIST

#include <functional>
#include <algorithm>
#include <iterator>

#include "debug.h"


// there are two different iterators in this container, "loop_iterator" and the common one "iterator"
// the differences between them are how to treat the end() iterator
// for the iterator :
// the end() is actually a nullptr,
// it's merely a tag used in the guard (iter != end()) to stop the loop 
// after accessing the node immediately previous to head.
// to achieve this, we also change iter to nullptr when it try to access the head for the second time
// for the loop_iterator :
// end() always equals to begin(),
// loop_iterator can be used to iterate from tail to head FORWARD while iterator will stop at end()
// function :
// as a result, we have to apply two sets of algorithm function for this two different iterator.
// the functions dealing with iterator work just the same as the STL function with a iter != end guard,
// while the functions dealing with loop_iterator, for example do_something(_start, _terminal),
// will not stop immediately after called when _start == _terminal,
// but will stop when iter try to access the _start for the second time.
// and of course, when _start != _terminal, common stl algorithm can be used perfectly for loop_iterator,
// and the iter will not stop even it come cross end()
namespace dyb
{
    using std::function;
    
    template<class EleType>
    struct double_linked_list_node
    {
        EleType _ele;
        double_linked_list_node * prev, *next;
        double_linked_list_node(EleType element)
            : _ele(element), prev(nullptr), next(nullptr)
        {
        }
    };

    template<class EleType>
    class iterator : public std::iterator<std::forward_iterator_tag, EleType>
    {
    public:
        typedef double_linked_list_node<EleType> node;

        explicit iterator(const node * head_node)
            : _ptr(nullptr), _head(head_node)
        {
        }

        iterator(const node * head_node, node * p_node)
            : _ptr(p_node), _head(head_node)
        {
        }

        const iterator & operator ++ ()
        {
            // _ptr should not be null
            if (_ptr->next == _head) _ptr = nullptr;
            else _ptr = _ptr->next;
            return *this;
        }

        const iterator operator ++ (int)
        {
            iterator temp(*this);
            if (_ptr->next == _head) _ptr = nullptr;
            else _ptr = _ptr->next;
            return temp;
        }

        bool operator == (iterator other)
        {
            return _ptr == other._ptr
                && _head == other._head;
        }

        bool operator != (iterator other)
        {
            return !(*this == other);
        }

        EleType & operator * ()
        {
            return _ptr->_ele;
        }

        EleType * operator -> ()
        {
            return _ptr;
        }

        node * get() const
        {
            return _ptr;
        }

    private:
        node * _ptr;
        const node * const _head;
    };

    template<class EleType>
    class loop_iterator : public std::iterator<std::forward_iterator_tag, EleType>
    {
    public:
        typedef double_linked_list_node<EleType> node;
        loop_iterator()
            : _ptr(nullptr)
        {}

        explicit loop_iterator(node * p_node)
            : _ptr(p_node)
        {
        }

        const loop_iterator & operator ++ () // should not be called when _ptr == nullptr
        {
            _ptr = _ptr->next;
        }

        const loop_iterator operator++ (int)
        {
            node * temp = _ptr;
            _ptr = _ptr->next;
            return temp;
        }

        bool operator == (loop_iterator other)
        {
            return _ptr == other._ptr;
        }
        bool operator != (loop_iterator other)
        {
            return _ptr != other._ptr;
        }

        EleType & operator * ()
        {
            return _ptr->_ele;
        }

        EleType * operator -> ()
        {
            return _ptr;
        }

        node * get() const
        {
            return _ptr;
        }
    private:
        node * _ptr;
    };

    // both are not implemented now and I use pointer instead
    template<class EleType>
    class circular_list
    {
    public:
        typedef double_linked_list_node<EleType> node;
        typedef iterator<EleType> common_iter;
        typedef loop_iterator<EleType> loop_iter;

        common_iter insert(common_iter location, const EleType & element)
        {
            return common_iter(head, insert(location.get(), element));
        }
        common_iter erase(common_iter location)
        {
            return common_iter(erase(location.get()));
        }
        common_iter find_if(common_iter _begin, common_iter _end, function<bool(const EleType & e)> pred)
        {
            return common_iter(std::find_if(_begin, _end, pred));
        }
        bool exist(common_iter iter)
        {
            return exist(iter.get());
        }

        loop_iter insert(loop_iter location, const EleType & element)
        {
            return loop_iter(insert(location.get(), element));
        }
        loop_iter erase(loop_iter location)
        {
            return loop_iter(erase(location.get()));
        }
        loop_iter find_if(loop_iter _begin, loop_iter _end, function<bool(const EleType & e)> pred)
        {
            return loop_iter(find_if(_begin.get(), _end.get(), pred));
        }
        bool exist(loop_iter iter)
        {
            return exist(iter.get());
        }

        size_t size() const { return _size; }
        common_iter begin() { return common_iter(head, head); }
        common_iter end() { return common_iter(head, nullptr); }
        loop_iter loop_begin() { return loop_iter(head); }
        loop_iter loop_end() { return loop_iter(head); }
    private:
        node * insert(node * location, const EleType & element);
        node * erase(node * location);
        // return nullptr when not found
        node * find_if(node * _begin, node * _end, function<bool(const EleType &)> pred);
        bool exist(node * p_node);

        node * head = nullptr;
        int _size = 0;
    };

    template<class EleType>
    typename circular_list<EleType>::node * circular_list<EleType>::insert(
        typename circular_list<EleType>::node * location, const EleType & element)
    {
        typedef typename circular_list<EleType>::node _MyNode;
        if (head == nullptr)
        {
            DEBUGCHECK(location == nullptr,
                "circular_list is empty but location is not nullptr");
            head = new _MyNode(element);
            head->next = head;
            head->prev = head;
            _size = 1;
            return head;
        }
        else if (location != nullptr)
        {
            DEBUGCHECK(exist(location), "location is not in the circular_list");
            _MyNode * left = location->prev;
            _MyNode * p = new _MyNode(element);
            left->next = p;
            p->prev = left;
            p->next = location;
            location->prev = p;
            ++_size;
            if (head == location)
                head = p;
            return p;
        }
        else // if (location == nullptr) // head != nullptr
        {
            _MyNode * left = head->prev;
            _MyNode * p = new _MyNode(element);
            left->next = p;
            p->prev = left;
            p->next = head;
            head->prev = p;
            ++_size;
            return p;
        }
    }

    template<class EleType>
    typename circular_list<EleType>::node * circular_list<EleType>::erase(
        typename circular_list<EleType>::node * location)
    {
        DEBUGCHECK(head != nullptr, "erase a node on a empty circular_list");
        DEBUGCHECK(exist(location), "location is not in the circular_list");
        typename circular_list<EleType>::node * next = location->next;
        location->prev->next = location->next;
        location->next->prev = location->prev;
        --_size;
        if (_size == 0) head = nullptr;
        else if (head == location) head = head->next;
        delete location;
        if (_size == 0) return nullptr;
        else next;
    }

    // work for loop_iterator
    template<class EleType>
    typename circular_list<EleType>::node * circular_list<EleType>::find_if(
        typename circular_list<EleType>::node * _begin,
        typename circular_list<EleType>::node * _end,
        function<bool(const EleType &)> pred)
    {
        bool tag = _begin == _end;
        for (typename circular_list<EleType>::node * p = _begin; p != _end || tag; p = p->next)
        {
            if (pred(p->_ele)) return p;
            tag = false;
        }
        return nullptr;
    }

    template<class EleType>
    bool circular_list<EleType>::exist(typename circular_list<EleType>::node * p_node)
    {
        if (head == nullptr) return false;
        typename circular_list<EleType>::node * p = head;
        do
        {
            if (p == p_node) return true;
            p = p->next;
        } while (p == head);
        return false;
    }

}

#endif
