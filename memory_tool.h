/* implement some memory process tool, similar to <stl_uninitialized> and <stl_allocator> 's implementation
 * 1. uninitialized_copy
 * 2. uninitialized_fill
 * 3. uninitialized_fill_n 
 * 
 * author: zhaoqf
 */

#ifndef __ZSTL_UNINITIALIZED__
#define __ZSTL_UNINITIALIZED__

#include "zconfig.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>


_ZSTL_NAMESPACE_BEGIN


/* construct object at given memory adress, also named placement-new
 * T1 means the object type, T2 means the construct argument type
 * if T2 equals T1, will call copy constructor, otherwise, call constructor
 */
template <typename T1, typename T2>
inline void _construct(T1* address, const T2& x) // use the T1* not void* because we expect deduce the T1 type with argument
{
    new(address) T1(x);
}


/* deconstruct object but not free memory
 * T is the type of the object
 */
template <typename T>
inline void _destroy(T* ptr)
{
    ptr->~T();  // deconstruct it
}


/* allocate the memory
 * we use T* here to indicate the type of object
 */
template <typename T>
inline void* _allocate(size_t size, T*)
{
    void* ptr = malloc(sizeof(T)*size);
    if(!ptr)
    {
        std::cerr << "_allocate:  out of memory!" << std::endl;
        exit(-1);
    }
    return ptr;
}



/* free the allocated memory
 */
template <typename T>
inline void _deallocate(T* ptr)
{
    free(ptr);  // just free the allocated memory
}


/* destory from start to end
*/
template <typename ForwardIterator>
inline void destroy(ForwardIterator start, ForwardIterator finish)
{
    auto cur = start;
    while (cur != finish)
    {
        _destroy(&(*cur));
        ++cur;
    }

}



/* copy the content from first to last to result 
 * it means every iterator will call construct(&*(result+(i-first)),*i)
 * the content iterator point to is not necessary to be continous
 * notes: InputIterator means we can only read the content, FowrdIterator means we can read & write but can 
 * only move the iterator forward
 */

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
{   auto cur_src = first;
    auto cur_des = result;

    while(cur_src!=last)
    {
        _construct(&(*cur_des), *cur_src);
        ++cur_src;  // move the src iterator
        ++cur_des;   // move the des iterator
    }
}


/* all the iterator between first and last, will call construct(&*i,x)
 */
template <typename ForwardIterator, typename T>
inline ForwardIterator uninitalized_fill(ForwardIterator first, ForwardIterator last, const T& x)
{
    auto cur_des = first;
    while(cur_des!=last)
    {
        _construct(&(*cur_des),x);
        ++cur_des;
    }

}


/* very similar to uninitalized_fill. the construction range is the `first` iterator and `first+n` iterator
*/
template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitalized_fill_n(ForwardIterator des, Size n, const T& x)
{
    auto cur_des = des;
    while(n--)
    {
        _construct(&(*cur_des), x);
        ++cur_des; 
    }
}


_ZSTL_NAMESPACE_END
#endif