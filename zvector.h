#ifndef __ZVECTOR__
#define __ZVECTOR__
#include <bits/allocator.h>
#include <initializer_list>
#include <cstdlib>
#include <iostream>
#include "zconfig.h"
#include "memory_tool.h"


/* A vector class created by zhaoqf. use stl allocator and initializer_list
 */
_ZSTL_NAMESPACE_BEGIN

template <typename _Tp, typename _Alloc = int>
class zvector
{
public:
    typedef _Tp value_type;
    typedef _Tp* pointer_type;
    typedef _Tp& reference_type;
    typedef pointer_type iterator;  // because of this is a vector, it can use pointer_type as iterator
    
private:
    iterator start;     // the begin iterator
    iterator finish;       // the end iterator
    iterator end_of_storage;    // the end of storage

public:
    zvector():start(nullptr),finish(nullptr),end_of_storage(nullptr)   // set the begin and end to nullptr
    {}


    zvector(size_t n, const _Tp& x)
    {
        
        // TODO: allocate from the allocator
        start = (pointer_type)zstd::_allocate(n,static_cast<pointer_type>(nullptr));

        finish = start;
        while(n--)
        {
            zstd::_construct(finish, x);
            ++finish;
        }
        end_of_storage = finish;
    }


    // copy constructor
    zvector(const zvector& rhd)
    {
        // self-assignment
        if (&rhd == this)
        {
            return ;
        }
        size_t n = rhd.size();
        start = (pointer_type)zstd::_allocate(n,static_cast<pointer_type>(nullptr));

        finish = start;
        for(size_t i = 0; i < n; i++)
        {
            zstd::_construct(finish, rhd[i]);
            ++finish;
        }
        end_of_storage = finish;

    }

//    support c++11 initializer list
#   if __cplusplus >= 201103L
    zvector(const std::initializer_list<_Tp>& il)
    {
        size_t n = il.size();
        start = static_cast<pointer_type>(zstd::_allocate(n,static_cast<pointer_type>(nullptr)));
        finish = start;

        for (auto& x: il)
        {
            zstd::_construct(finish, x);
            ++finish;
        }

        end_of_storage = finish;
    }
#   endif 



    // destory all object and than free the memory
    ~zvector()
    {
        if(start!=end_of_storage)
        {
            destroy(start,finish);
            free(start);
        }
    }


    // return the begin iterator
    iterator begin() const 
    { 
        return start;
    }


    // return the end iterator
    iterator end() const
    {
        return finish;
    }

    value_type& operator[](int idx) const
    {
        return *(start+idx);
    }


    // push a item back of the vector. 
    void push_back(const reference_type x)
    {
        // 
        if (end_of_storage > finish)
        {
            uninitalized_fill_n(finish, 1, x);
        }
        else    // storage is not enough
        {
            size_t new_capacity = capacity() ? 2*capacity() : 1;
            size_t old_size = size();
            
            pointer_type p_new_memory = static_cast<pointer_type>(_allocate(new_capacity, static_cast<pointer_type>(nullptr)));
            uninitialized_copy(start,finish, p_new_memory);
            uninitalized_fill_n(p_new_memory + old_size, 1, x);

            // destroy and free old data
            if (start != end_of_storage)
            {
                destroy(start, finish);
                free(start);
            }

            start = p_new_memory;
            finish = start + old_size + 1;
            end_of_storage = start + new_capacity;

        }
    }


    void pop_back()
    {
        // has element
        if(finish != start)
        {
            _destroy(finish);
            --finish;
        }
    }


    // erase at a iterator
    void erase(iterator iter)
    {
        _destroy(iter);
        uninitialized_copy(iter+1, finish, iter); // copy the element after iter
        --finish;
    }


    // erase range [pos1, pos2)
    void erase(iterator iter_start, iterator iter_end)
    {
        auto cur = iter_start;
        while(cur!=iter_end)
        {
            _destroy(cur);
            ++cur;
        }
        uninitialized_copy(iter_end, finish, iter_start);
        finish -= (iter_end - iter_start); 
    }


    // insert
    void insert(iterator iter, const reference_type x)
    {
        
        // TODO: 

    }
    // clear
    void clear()
    {
        auto cur = start;
        while(cur != finish)
        {
            _destroy(cur);
            ++cur;
        }
        finish = start;
    }
    
    // is empty?
    bool empty()
    {
        if (finish == start)
        {
            return true;
        }
        return false;
    }

    // return the front item
    reference_type front()
    {
        return *start;
    }

    // return the back item
    reference_type back()
    {
        return *(finish-1);
    }

    // return the size of the vector
    size_t size() const
    {
        return size_t(finish - start);
    }


    // return the capacity of the vector
    size_t capacity() const
    {
        return size_t(end_of_storage - start);
    }


    // return the max_size of the vector
    size_t max_size()
    {
        // TODO: 
        return 100000;
    }
    
    // resize the storage
    void resize()
    {
        // TODO: 
    }

    friend std::ostream& operator<<(std::ostream& os, const zvector<value_type>& vec)
    {
        for (auto iter = vec.begin(); iter != vec.end(); iter++)
        {
            std::cout << *iter << " ";
        }
        std::cout << std::endl;
    }

};

_ZSTL_NAMESPACE_END

#endif

