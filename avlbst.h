#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError
{
};

/**
 * A special kind of node for an AVL tree, which adds the balance as a data member, plus
 * other additional helper functions. You do NOT need to implement any functionality or
 * add additional data members or helper functions.
 */

template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value> *getParent() const override;
    virtual AVLNode<Key, Value> *getLeft() const override;
    virtual AVLNode<Key, Value> *getRight() const override;

protected:
    int8_t balance_; // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
 * An explicit constructor to initialize the elements by calling the base class constructor
 */
template <class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent) : Node<Key, Value>(key, value, parent), balance_(0)
{
}

/**
 * A destructor which does nothing.
 */
template <class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{
}

/**
 * A getter for the balance of a AVLNode.
 */
template <class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
 * A setter for the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
 * Adds diff to the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
 * An overridden function for getting the parent since a static_cast is necessary to make sure
 * that our node is a AVLNode.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value> *>(this->parent_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value> *>(this->left_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value> *>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key &key);                              // TODO
protected:
    virtual void nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n);
    void rotateRight(AVLNode<Key, Value> *n);
    void rotateLeft(AVLNode<Key, Value> *n);
    bool checkZigZig(AVLNode<Key, Value> *g, AVLNode<Key, Value> *p, AVLNode<Key, Value> *n);
    void removeFix(AVLNode<Key, Value> *n, int diff);
};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // TODO

    // if tree is empty
    if (this->root_ == nullptr)
    {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    else
    {
        AVLNode<Key, Value> *currentNode = static_cast<AVLNode<Key, Value> *>(this->root_);
        AVLNode<Key, Value> *parentNode = nullptr;

        // iterate to place the node
        while (currentNode != nullptr)
        {
            if (new_item.first < currentNode->getKey())
            {
                parentNode = currentNode;
                currentNode = currentNode->getLeft();
            }

            else if (new_item.first > currentNode->getKey())
            {
                parentNode = currentNode;
                currentNode = currentNode->getRight();
            }

            else
            {
                currentNode->setValue(new_item.second);
                return;
            }
        }

        AVLNode<Key, Value> *newNode;

        // inserts the node
        if (new_item.first < parentNode->getKey())
        {
            newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parentNode);
            parentNode->setLeft(newNode);
        }
        else
        {
            newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parentNode);
            parentNode->setRight(newNode);
        }

        // does b(p) check
        if (parentNode->getBalance() == -1 || parentNode->getBalance() == 1)
        {
            parentNode->setBalance(0);
        }
        else
        {
            if (parentNode->getLeft() == newNode)
            {
                parentNode->setBalance(-1);
            }

            else if (parentNode->getRight() == newNode)
            {
                parentNode->setBalance(1);
            }

            insertFix(parentNode, newNode);
        }

        return;
    }
}
template <class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n)
{
    if (p == nullptr || p->getParent() == nullptr)
    {
        return;
    }

    AVLNode<Key, Value> *g = p->getParent();

    if (p == g->getLeft())
    {
        g->updateBalance(-1);

        if (g->getBalance() == 0)
        {
            return;
        }
        else if (g->getBalance() == -1)
        {
            insertFix(g, p);
        }
        else if (g->getBalance() == -2)
        {
            // if zig zig
            if (checkZigZig(p->getParent(), p, n))
            {
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            }

            // if zig zag
            else
            {
                rotateLeft(p);
                rotateRight(g);

                if (n->getBalance() == -1)
                {
                    p->setBalance(0);
                    g->setBalance(1);
                    n->setBalance(0);
                }

                else if (n->getBalance() == 0)
                {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }

                else if (n->getBalance() == 1)
                {
                    p->setBalance(-1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }
    }

    else if (p == g->getRight())
    {
        g->updateBalance(1);

        if (g->getBalance() == 0)
        {
            return;
        }
        else if (g->getBalance() == 1)
        {
            insertFix(g, p);
        }
        else if (g->getBalance() == 2)
        {
            // if zig zig
            if (checkZigZig(p->getParent(), p, n))
            {
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            }

            // if zig zag
            else
            {
                rotateRight(p);
                rotateLeft(g);

                if (n->getBalance() == 1)
                {
                    p->setBalance(0);
                    g->setBalance(-1);
                    n->setBalance(0);
                }

                else if (n->getBalance() == 0)
                {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }

                else if (n->getBalance() == -1)
                {
                    p->setBalance(1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *n)
{
    // n is grandparent
    AVLNode<Key, Value> *g = n;
    AVLNode<Key, Value> *p = n->getLeft();

    if (g->getParent() == nullptr)
    {
        g->setParent(p);

        if (p->getRight() == nullptr)
        {
            g->setLeft(nullptr);
        }

        else
        {
            g->setLeft(p->getRight());
            g->getLeft()->setParent(g);
        }

        p->setParent(nullptr);
        p->setRight(g);

        this->root_ = p;
    }

    else
    {
        p->setParent(g->getParent());

        if (g->getParent()->getLeft() == g)
        {
            g->getParent()->setLeft(p);
        }

        else if (g->getParent()->getRight() == g)
        {
            g->getParent()->setRight(p);
        }

        if (p->getRight() == nullptr)
        {
            g->setLeft(nullptr);
        }

        else
        {
            g->setLeft(p->getRight());
            g->getLeft()->setParent(g);
        }

        g->setParent(p);

        p->setRight(g);
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *n)
{
    // n is grandparent
    AVLNode<Key, Value> *g = n;
    AVLNode<Key, Value> *p = n->getRight();

    if (g->getParent() == nullptr)
    {
        g->setParent(p);

        if (p->getLeft() == nullptr)
        {
            g->setRight(nullptr);
        }

        else
        {
            g->setRight(p->getLeft());
            g->getRight()->setParent(g);
        }

        p->setParent(nullptr);
        p->setLeft(g);

        this->root_ = p;
    }

    else
    {
        p->setParent(g->getParent());

        if (g->getParent()->getRight() == g)
        {
            g->getParent()->setRight(p);
        }

        else if (g->getParent()->getLeft() == g)
        {
            g->getParent()->setLeft(p);
        }

        if (p->getLeft() == nullptr)
        {
            g->setRight(nullptr);
        }

        else
        {
            g->setRight(p->getLeft());
            g->getRight()->setParent(g);
        }

        g->setParent(p);

        p->setLeft(g);
    }
}

template <class Key, class Value>
bool AVLTree<Key, Value>::checkZigZig(AVLNode<Key, Value> *g, AVLNode<Key, Value> *p, AVLNode<Key, Value> *n)
{
    // check left zig zig
    if (g->getLeft() == p && p->getLeft() == n)
    {
        return true;
    }
    // check right zig zig
    else if (g->getRight() == p && p->getRight() == n)
    {
        return true;
    }

    return false;
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::remove(const Key &key)
{
    // TODO
    AVLNode<Key, Value> *n = static_cast<AVLNode<Key, Value> *>(this->internalFind(key));
    AVLNode<Key, Value> *p;
    int diff = 0;

    // case if there is no nodes in the BST
    if (n == nullptr)
    {
        return;
    }

    // if n has both children
    if (n->getLeft() && n->getRight())
    {
        AVLNode<Key, Value> *predecessor = n->getLeft();
        while (predecessor->getRight() != nullptr)
        {
            predecessor = predecessor->getRight();
        }

        nodeSwap(n, predecessor);
    }

    // let p = parent(n)
    p = n->getParent();

    // if p is not null
    if (p != nullptr)
    {
        if (n == p->getLeft())
        {
            diff = 1;
        }

        else if (n == p->getRight())
        {
            diff = -1;
        }
    }

    // remove code from BST
    AVLNode<Key, Value> *parentNode = n->getParent();
    AVLNode<Key, Value> *child = n->getLeft();
    if (n->getRight() != nullptr)
    {
        child = n->getRight();
    }

    if (n == this->root_)
    {
        this->root_ = child;
        if (child != nullptr)
        {
            child->setParent(nullptr);
        }
    }
    else
    {
        if (n == parentNode->getLeft())
        {
            parentNode->setLeft(child);
        }
        else
        {
            parentNode->setRight(child);
        }
        if (child != nullptr)
        {
            child->setParent(parentNode);
        }
    }

    delete n;

    removeFix(parentNode, diff);
}

template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value> *n, int diff)
{
    if (n == nullptr)
    {
        return;
    }

    AVLNode<Key, Value> *p = n->getParent();
    AVLNode<Key, Value> *c;
    AVLNode<Key, Value> *g;
    int ndiff = 0;
    if (p != nullptr)
    {
        if (p->getLeft() == n)
        {
            ndiff = 1;
        }
        else if (p->getRight() == n)
        {
            ndiff = -1;
        }
    }

    if (diff == -1)
    {
        if (n->getBalance() + diff == -2)
        {
            //[Perform the check for the mirror case where b(n) + diff == +2, flipping left/right and -1/+1]

            c = n->getLeft();

            if (c->getBalance() == -1)
            {
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }

            else if (c->getBalance() == 0)
            {
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
            }
            else if (c->getBalance() == 1)
            {
                g = c->getRight();
                rotateLeft(c);
                rotateRight(n);

                if (g->getBalance() == 1)
                {
                    n->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0)
                {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if (g->getBalance() == -1)
                {
                    n->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == -1)
        {
            n->setBalance(-1);
        }
        else if (n->getBalance() + diff == 0)
        {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }

    else if (diff == 1)
    {
        if (n->getBalance() + diff == 2)
        {
            //[Perform the check for the mirror case where b(n) + diff == +2, flipping left/right and -1/+1]

            c = n->getRight();

            if (c->getBalance() == 1)
            {
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }

            else if (c->getBalance() == 0)
            {
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            else if (c->getBalance() == -1)
            {
                g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);

                if (g->getBalance() == -1)
                {
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0)
                {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 1)
                {
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == 1)
        {
            n->setBalance(1);
        }
        else if (n->getBalance() + diff == 0)
        {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
