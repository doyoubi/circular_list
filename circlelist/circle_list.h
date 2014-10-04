#ifndef DYB_CIRCLE_LIST
#define DYB_CIRCLE_LIST

#include <type_traits>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <iterator>

#include "debug.h"


// There are two different iterators in this container, "loop_iterator" and the common one "common_iterator"
// The differences between them are how to treat the end() iterator
// and their behavior when a iter increment to end().

// for the common_iterator :
// The end() is actually a nullptr,
// It's merely a tag used in the guard (iter != end()) to stop the loop
// after accessing the node immediately previous to head,
// or a tag indicating element not found when using search function such as find, find_if.
// To achieve this, we change iter to nullptr after it accessing the node immediately before the head node.

// for the loop_iterator :
// end() always equals to begin().
// loop_iterator can be used to iterate from tail to head FORWARD while common_iterator will stop at end().
// But loop_end CAN NOT used to indicate that not being able to find someting in the container.
// When circular_list::find_if(loop_iterator version) can't find a element in container,
// it return null loop_iterator which is NOT the same as loop_end.
// And extra operator==() will be provided to let null loop_iterator equals end() of common_iterator

// function :
// For this two kind of iterator, we must be careful when using function for them.
// The STL algorithm function work fine for common_iterator.
// The (iter != end) guard will stop the loop correctly and end() can be used to indicate element not found.
// But when using loop_iter in such function with (iter != end) guard,
// find(_begin, _end, value) for example, 
// the loop will not start when _begin == _end, of which using loop_begin() and loop_end() is a case.
// And of course, when _begin != _end, common stl algorithm can be used perfectly for loop_iterator,
// and the iter will not stop even it come cross end().
// So I provide a customed find_if for loop_iterator to deal with the _begin == _end case,
// which is a member of circular_list.
// It will not stop the loop immediately after called when _start == _terminal,
// but will stop when iter try to access the _start for the second time.


namespace dyb
{
    using std::function;
    
    template<class EleType>
    struct double_linked_list_node
    {
        EleType _ele;
        double_linked_list_node * prev, *next;
        double_linked_list_node(const EleType & element)
            : _ele(element), prev(nullptr), next(nullptr)
        {
        }
    };

    template<class EleType, bool is_const>
    class common_iterator : public std::iterator<std::forward_iterator_tag, EleType>
    {
    public:
        typedef double_linked_list_node<EleType> node;
        // both const and non const
        typedef typename std::conditional<is_const, const EleType, EleType>::type cncEleType;
        typedef typename std::conditional<is_const, const node, node>::type cncNode;

        explicit common_iterator(const node * head_node)
            : _ptr(nullptr), _head(head_node)
        {
        }

        common_iterator(const node * head_node, node * p_node)
            : _ptr(p_node), _head(head_node)
        {
        }

        common_iterator & operator ++ ()
        {
            CHECKNULL(_ptr);
            if (_ptr->next == _head) _ptr = nullptr;
            else _ptr = _ptr->next;
            return *this;
        }

        common_iterator operator ++ (int)
        {
            CHECKNULL(_ptr);
            common_iterator temp(*this);
            if (_ptr->next == _head) _ptr = nullptr;
            else _ptr = _ptr->next;
            return temp;
        }

        bool operator == (common_iterator other)
        {
            return _ptr == other._ptr; // _head must be the same
        }

        bool operator != (common_iterator other)
        {
            return !(*this == other);
        }

        cncEleType & operator * ()
        {
            CHECKNULL(_ptr);
            return _ptr->_ele;
        }

        cncEleType * operator -> ()
        {
            CHECKNULL(_ptr);
            return &(_ptr->_ele);
        }

        cncNode * get() const
        {
            return _ptr;
        }

        friend class common_iterator<EleType, true>;

    private:
        node * _ptr;
        const node * _head;
    };

    template<class EleType, bool is_const>
    class loop_iterator : public std::iterator<std::forward_iterator_tag, EleType>
    {
    public:
        typedef double_linked_list_node<EleType> node;
        // both const and non const
        typedef typename std::conditional<is_const, const EleType, EleType>::type cncEleType;
        typedef typename std::conditional<is_const, const node, node>::type cncNode;
        loop_iterator()
            : _ptr(nullptr)
        {
        }

        explicit loop_iterator(node * p_node)
            : _ptr(p_node)
        {
        }

        loop_iterator & operator ++ () // should not be called when _ptr == nullptr
        {
            CHECKNULL(_ptr);
            _ptr = _ptr->next;
            return *this;
        }

        loop_iterator operator++ (int)
        {
            CHECKNULL(_ptr);
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

        cncEleType & operator * ()
        {
            CHECKNULL(_ptr);
            return _ptr->_ele;
        }

        cncEleType * operator -> ()
        {
            CHECKNULL(_ptr);
            return &(_ptr->_ele);
        }

        cncNode * get() const
        {
            return _ptr;
        }

        friend class loop_iterator<EleType, true>;

    private:
        node * _ptr;
    };

    // comparasion between common_iterator and loop_iterator
    template<class EleType, bool common_iter_is_const, bool loop_iter_is_const>
    bool operator == (
        const common_iterator<EleType, common_iter_is_const> & lhs,
        const loop_iterator<EleType, loop_iter_is_const> & rhs)
    {
        return lhs.get() == rhs.get();
    }

    template<class EleType, bool common_iter_is_const, bool loop_iter_is_const>
    bool operator == (
        const loop_iterator<EleType, loop_iter_is_const> & lhs,
        const common_iterator<EleType, common_iter_is_const> & rhs)
    {
        return rhs == lhs;
    }

    template<class EleType, bool common_iter_is_const, bool loop_iter_is_const>
    bool operator != (
        const common_iterator<EleType, common_iter_is_const> & lhs,
        const loop_iterator<EleType, loop_iter_is_const> & rhs)
    {
        return lhs.get() != rhs.get();
    }

    template<class EleType, bool common_iter_is_const, bool loop_iter_is_const>
    bool operator != (
        const loop_iterator<EleType, loop_iter_is_const> & lhs,
        const common_iterator<EleType, common_iter_is_const> & rhs)
    {
        return rhs != lhs;
    }

    // circular_list
    template<class EleType>
    class circular_list
    {
    public:
        typedef double_linked_list_node<EleType> node;
        typedef common_iterator<EleType, false> iterator;
        typedef common_iterator<EleType, true> const_iterator;
        typedef common_iterator<EleType, false> common_iter;
        typedef common_iterator<EleType, true> const_common_iter;
        typedef loop_iterator<EleType, false> loop_iter;
        typedef loop_iterator<EleType, true> const_loop_iter;

        // helper function
        circular_list(std::initializer_list<EleType> _initList)
        {
            for (auto & ele : _initList)
            {
                insert(nullptr, ele);
            }
        }

        // since const iterator is not implemented so use non const reference
        circular_list(const circular_list & other)
            : head(nullptr), _size(other._size)
        {
            for (auto & ele : other)
            {
                insert(nullptr, ele);
            }
        }

        // move constructor
        circular_list(circular_list && other)
            : head(other.head), _size(other._size)
        {
            other.head = nullptr;
            other._size = 0;
        }

        circular_list & operator = (const circular_list & other)
        {
            DEBUGCHECK(this != &other, "assignment to self");
            clear();
            _size = other._size;
            for (auto & ele : other)
            {
                insert(nullptr, ele);
            }
            return *this;
        }

        // move assignment
        circular_list & operator = (circular_list && other)
        {
            DEBUGCHECK(this != &other, "assignment to self");
            clear();
            head = other.head;
            other.head = nullptr;
            _size = other._size;
            other._size = 0;
            return *this;
        }

        circular_list::~circular_list()
        {
            clear();
        }

        common_iter insert(common_iter location, const EleType & element)
        {
            return common_iter(head, insert(location.get(), element));
        }
        common_iter erase(common_iter location)
        {
            return common_iter(head, erase(location.get()));
        }
        common_iter find_if(common_iter _begin, common_iter _end, function<bool(const EleType & e)> pred)
        {
            return std::find_if(_begin, _end, pred);
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

        void clear()
        {
            if (head == nullptr) return;
            head->prev->next = nullptr;
            node * p = head;
            while (p != nullptr)
            {
                node * temp = p;
                p = p->next;
                delete temp;
            }
            _size = 0;
        }
        size_t size() const { return _size; }
        common_iter begin() { return common_iter(head, head); }
        common_iter end() { return common_iter(head, nullptr); }
        const_common_iter begin() const { return const_common_iter(head, head); }
        const_common_iter end() const { return const_common_iter(head, nullptr); }

        loop_iter loop_begin() { return loop_iter(head); }
        loop_iter loop_end() { return loop_iter(head); }
        const_loop_iter loop_begin() const { return const_loop_iter(head); }
        const_loop_iter loop_end() const { return const_loop_iter(head); }

        circular_list() = default;

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
                "circular_list::insert: circular_list is empty but location is not nullptr");
            head = new _MyNode(element);
            head->next = head;
            head->prev = head;
            _size = 1;
            return head;
        }
        else if (location != nullptr)
        {
            DEBUGCHECK(exist(location), "circular_list::insert: location is not in the circular_list");
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
        DEBUGCHECK(head != nullptr, "circular_list::erase: erase a node on a empty circular_list");
        DEBUGCHECK(exist(location), "circular_list::erase: location is not in the circular_list");
        typename circular_list<EleType>::node * next = location->next;
        location->prev->next = location->next;
        location->next->prev = location->prev;
        --_size;
        if (_size == 0) head = nullptr;
        else if (head == location) head = head->next;
        delete location;
        if (_size == 0) return nullptr;
        else return next;
    }

    // work for loop_iterator
    template<class EleType>
    typename circular_list<EleType>::node * circular_list<EleType>::find_if(
        typename circular_list<EleType>::node * first,
        typename circular_list<EleType>::node * last,
        function<bool(const EleType &)> pred)
    {
        // precondition: both first and last point to a node of the same circular_list
        DEBUGCHECK(exist(first), "invalid first pointer");
        DEBUGCHECK(exist(last), "invalid last pointer");
        CHECKNULL(pred);
        do
        {
            if (pred(first->_ele)) return first;
            first = first->next;
        } while (first != last);
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
        } while (p != head);
        return false;
    }


    // customed algorithm for loop_iterator
    template<class EleType, class Pred, bool is_const>
    typename loop_iterator<EleType, is_const> adjacent_find(
        loop_iterator<EleType, is_const> first, 
        loop_iterator<EleType, is_const> last,
        Pred pred)
    {
        auto next = first; ++next;
        do
        {
            if (pred(*first, *next))
                return first;
            ++first;
            ++next;
        } while (first != last);
        return loop_iterator<EleType, is_const>(nullptr);
    }

    template<class EleType, class Function, bool is_const>
    Function for_each(
        loop_iterator<EleType, is_const> first,
        loop_iterator<EleType, is_const> last,
        Function func)
    {
        do
        {
            func(*first);
            ++first;
        } while (first != last);
        return std::move(func);
    }

    template<class EleType, class Function, bool is_const>
    Function for_adjacent(
        loop_iterator<EleType, is_const> first,
        loop_iterator<EleType, is_const> last,
        Function func)
    {
        auto next = first; ++next;
        do
        {
            func(*first, *next);
            ++first;
            ++next;
        } while (first != last);
        return std::move(func);
    }

}

#endif
