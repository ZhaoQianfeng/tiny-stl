/* implementation of rb tree */
#ifndef __RBTREE__
#define __RBTREE__

#include <zconfig.h>
#include <memory_tool.h>

_ZSTL_NAMESPACE_BEGIN


typedef bool __rb_tree_color_type;  
const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black = true;

struct __rb_tree_node_base
{
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;

    color_type color;
    base_ptr parent;    // rb tree的很多操作必须知道父节点
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x)
    {
        while(x->left != 0) x = x -> left;
        return x;
    }

    static base_ptr maximum(base_ptr x)
    {
        while(x->right != 0) x = x -> right;
        return x;
    }
};


template <typename Value>
struct __rb_tree_node : public __rb_tree_node_base
{
    typedef __rb_tree_node<Value>* link_type;   // 指针类型
    Value value_field;  // 数据域
};




struct __rb_tree_base_iterator
{
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag  iterator_category;
    typedef ptrdiff_t difference_type;

    base_ptr node; //用来指向当前node

    void increment()
    {
        if (node->right != 0)   // 如果有右子节点
        {
            node = node->right;
            while (node->left != 0)
            {
                node = node->left;
            }

        }
        else
        {
            base_ptr y = node->parent;
            while (node == y->right)
            {
                node = y;
                y = node->parent;
            }
            if (node->right != y )      // 这是一种特殊情况，等会说
            {
                node = y;   // 
            }
        }
    }


    void decrement()
    {
        // 红节点，且祖父节点等于自己时。这是根节点的情况
        if (node->color == __rb_tree_red && node->parent->parent == node)
        {
            node = node->right;
        }else if (node->left!=0)
        {
            base_ptr y = node->left;    //左子树的最右节点
            while(y->right != 0)
            {
                y = y->right;
                node = y;
            }
        }else   // 既不是根节点，也没有左子节点
        {
            base_ptr y = node->parent;
            while(node==y->left)
            {
                node = y;
                y = y->parent;
            }
            node = y;
        }
    }


};
// rb-tree正规迭代器
template <typename Value, typename Ref, typename Ptr>
struct __rb_tree_iterator : public __rb_tree_iterator_base
{
    typedef Value value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef __rb_tree_iterator<Value,Value&,Value*> iterator;
    // const iterator不允许修改迭代器所指向内容
    typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator; 
    typedef __rb_tree_iterator<Value, Ref, Ptr> self;
    typedef __rb_tree_node<Value>* link_type;

    __rb_tree_iterator() {}
    __rb_tree_iterator(link_type x) {node = x}
    __rb_tree_iterator(const iterator& it) { node = it.node}

    reference operator*() const { return link_type(node)->value_field;}
    pointer operator->() {return &(operator*());}

    self& operator++() {increment(); return *this};
    // 后置++,返回右值
    self operator++(int)
    {
        self tmp = *this;
        increment();
        return tmp;
    }

    self& operator--() {decrement(); return *this};

    self operator--(int)
    {
        self tmp = *this;
        decrement();
        return tmp;
    }

};


// rb tree的定义
template <class Key, class Value, class KeyOfValue,  class Compare, class Alloc = alloc>
class rb_tree
{
protected:
    typedef void* void_pointer;
    typedef __rb_tree_node_base* base_ptr;
    typedef __rb_tree_node<Value> rb_tree_node;
    typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
protected:
    typedef Key key_type;
    typedef Value value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef rb_tree_ndoe* link_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

protected:
    link_type get_node()
    {
        return rb_tree_node_allocator::allocator();
    }
    
    void put_node(link_type p)
    {
        rb_tree_node_allocator::deallocator(p);
    }

    link_type create_node(const value_type& x)
    {
        link_type tmp = get_node();     // 获取空间
        construct(&tmp->value.field, x); // 调用构造函数
        return tmp; 
    }

    // 克隆一个节点，节点的颜色不变
    link_type clone_node (link_type x)
    {
        link_type tmp = create_node(x->value_field);
        // 设置颜色和左右子节点
        tmp->color = x->color;
        tmp->right = 0;
        tmp->left = 0;
        return tmp;
    }

    void destroy_node (link_node x)
    {
        _destroy(&x->value_field);  // 析构
        put_node(x);    // 释放内存
    }

protected:
    // rb-tree的三个属性
    size_type node_count;  // 记录记录树的大小
    link_type header; // 跟踪rb-tree的访问点
    Compare key_compare; // 节点的键值比较准则，是一个function-like object

    // 访问header的成员
    link_type& root() { return (link_type&) header->parent;}    // 返回指针的引用，意味着后面可能要改指针的指向
    link_type& leftmost() { return (line_type&) header->left;}  
    link_type& rightmost() { return (link_type&) header->right;}
    
    // 下面六个函数访问某个节点x的成员
    static link_type& left(link_type x)
    {
        return (line_type&) (x->left);
    }
    static link_type& right(link_type x)
    {
        return (line_type&) (x->right);
    }
    static link_type& parent(link_type x)
    {
        return (line_type&) (x->parent);
    }
    static reference value (link_type x)
    {
        return x->value_field;
    }
    static const Key& key(link_type x)
    {
        return KeyOfValue()(value(x));
    }
    static color_type& color(link_type)
    {
        return (color_type&)(x->color);
    }

    //求取以某个节点为树根的子树最大值和最小值
    static link_type minimum(link_type x)
    {
        return (link_type) __rb_tree_node_base::minimum(x);
    }
    static link_type minimum(link_type x)
    {
        return (link_type) __rb_tree_node_base::maximim(x);
    }


public:
    // 实例化的rb-tree有自己对应的迭代器类型
    typedef __rb_tree_iterator<value_type, reference, pointer> iterator;

private:
    iterator __insert(base_ptr x, base_ptr y, const value_type& x);
    link_type __copy(link_type x, link_type p);
    void __erase(link_type x);
    void init()
    {
        header = get_node();
        color(header) = __rb_tree_red;  // 令header为红色
        root() = 0; 
        leftmost() = header;
        rightmost() = header; // 一开始header的左右节点都指向自己
    }

public:
    rb_tree(const Compare& comp = Compare()):node_count(0), key_compare(comp) { init(); }
    ~rb_tree()
    {
        clear();
        put_node(header);
    }
    rb_tree& operator=(const rb_tree& x);
    // 禁用拷贝构造
    rb_tree& rb_tree(const rb_tree& x) = delete;

public:
    Compare key_comp() const { return key_compare; }
    iterator begin() { return leftmost(); }
    iterator end() { return header; }
    bool empty() const {return node_count == 0;}
    size_type size() const { return node_count; }
    size_type max_size() const  { return size_type(-1); }

public:
// 将x插入到rb-tree中，保持节点值独一无二
pair<iterator,bool> insert_unique(const value_type& x);
// 将x插入到rb-tree中 （允许节点值重复）
iteator insert_equal (const value_type& x);
    

}






_ZSTL_NAMESPACE_END
#endif