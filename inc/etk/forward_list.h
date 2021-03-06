
#ifndef ETK_FORWARD_LIST_H
#define ETK_FORWARD_LIST_H


#include <etk/etk.h>

namespace etk
{

template <typename T, uint32 SZ> class forward_list
{
private:
    struct Node;

public:
    class Iterator
    {
        friend class forward_list;
    public:
        Iterator() { }

        Iterator(forward_list::Node* head) : node(head) { }
        void next()
        {
            node = node->next;
        }

        T& operator*()
        {
            return node->data;
        }

        Iterator& operator++ ()
        {
            node = node->next;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator iter(*this);
            ++(*this);
            return iter;
        }

        bool operator==(Iterator iter)
        {
            return (node == iter.node);
        }

        bool operator!=(Iterator iter)
        {
            return (node != iter.node);
        }

    private:
        forward_list::Node* node;
    };

    forward_list(MemPool<SZ>& pool) : pool(pool)
    {
    }

    ~forward_list()
    {
        Node* node = head;
        while(node != nullptr)
        {
            Node* pnext = node->next;
            pool.free(node);
            node = pnext;
        }
    }


    Iterator begin()
    {
        return Iterator(head);
    }

    Iterator end()
    {
        return Iterator(nullptr);
    }

    void append()
    {
        T t = T();
        append(t);
    }

    void append(T t)
    {
        Node* node = static_cast<Node*>(pool.alloc(sizeof(Node)));
        node->data = t;
        node->next = nullptr;
        if(head == nullptr)
        {
            head = node;
            return;
        }

        Node* pnode = head;
        while(pnode->next)
            pnode = pnode->next;

        pnode->next = node;
    }

    void insert_before(Iterator iter, T t)
    {
        Node* node = static_cast<Node*>(pool.alloc(sizeof(Node)));
        node->data = t;

        if(iter.node == head)
        {
            node->next = head;
            head = node;
            return;
        }

        Node* pnode = head;
        uint32 count = 0;
        while(pnode->next)
        {
            if(iter.node == pnode->next)
            {
                node->next = pnode->next;
                pnode->next = node;
                return;
            }
            pnode = pnode->next;
            count++;
        }

        pool.free(node);
        pool.coalesce_free_blocks();
        return;
    }

    void insert_after(Iterator iter, T t)
    {
        Node* node = static_cast<Node*>(pool.alloc(sizeof(Node)));
        node->data = t;

        Node* pnext = iter.node->next->next;
        iter.node->next = node;
        node->next = pnext;
    }

    void remove_item(T t)
    {
        Node* node = head;

        if(node->data == t)
        {
            node = node->next;
            pool.free(head);
            pool.coalesce_free_blocks();
            head = node;
            return;
        }

        while(node->next)
        {
            if(node->next->data == t)
            {
                Node* pnext = node->next->next;
                pool.free(node->next);
                pool.coalesce_free_blocks();
                node->next = pnext;
                return;
            }
            node = node->next;
        }
        return;
    }

    void remove_n(uint32 n)
    {
        uint32 c = 0;
        if(n == 0)
        {
            if(head)
            {
                Node* pnext = head->next;
                pool.free(head);
                head = pnext;
            }
            return;
        }

        Node* node = head;
        while(node->next)
        {
            c++;
            if(c > n)
                return;
            if(n == c)
            {
                Node* pnext = node->next->next;
                pool.free(node->next);
                node->next = pnext;
                return;
            }
            node = node->next;
        }
        return;
    }

    T* get(uint32 n)
    {
        if(n == 0)
            return head;

        Node* node = head;
        uint32 c = 0;
        while(node->next)
        {
            c++;
            if(c > n)
                return nullptr;
            if(n == c)
                return node->next;
            node = node->next;
        }
        return nullptr;
    }

private:
    struct Node
    {
        T data;
        Node* next = nullptr;
    };

    Node* head = nullptr;
    MemPool<SZ>& pool;
};

}


#endif
