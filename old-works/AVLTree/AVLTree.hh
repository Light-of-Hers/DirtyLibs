#ifndef __CRZ_AVLTREE_HH__
#define __CRZ_AVLTREE_HH__

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>

namespace crz {

template <class T>
class AvlTree {
    typedef struct ELE {
        T val;
        ELE *child[2];
        ELE *parent;
        int height;
        ELE(const T &v, ELE *c1, ELE *c2, ELE *p, int h)
            : val(v), parent(p), height(h) {
            child[0] = c1;
            child[1] = c2;
        }
    } * Node, *Tree;
    Tree root;
    // -------------------------------
    // only const_iterator for it's meaningless to modify a node of a
    // balanced-tree
  public:
    class const_iterator {
      protected:
        Node cur;

      public:
        const_iterator(Node p) : cur(p) {}
        const_iterator() : cur(nullptr) {}
        const_iterator(const const_iterator &rhs) : cur(rhs.cur) {}
        const_iterator &operator=(const const_iterator &rhs) {
            cur = rhs.cur;
            return *this;
        }
        const_iterator &operator++() {
            assert(cur);
            Node dest = _min(cur->child[1]);
            if (dest) {
                cur = dest;
            } else {
                for (dest = cur->parent; dest && dest->val < cur->val;
                     dest = dest->parent) {
                }
                cur = dest;
            }
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        const_iterator &operator--() {
            assert(cur);
            Node dest = _max(cur->child[0]);
            if (dest) {
                cur = dest;
            } else {
                for (dest = cur->parent; dest && dest->val > cur->val;
                     dest = dest->parent) {
                }
                cur = dest;
            }
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp(*this);
            --(*this);
            return tmp;
        }
        friend bool operator==(const const_iterator &lhs,
                               const const_iterator &rhs) {
            return lhs.cur == rhs.cur;
        }
        friend bool operator!=(const const_iterator &lhs,
                               const const_iterator &rhs) {
            return !(lhs == rhs);
        }
        const T &operator*() { return cur->val; }
        const T *&operator->() { return &this->operator*(); }
    };
    const_iterator begin() const { return const_iterator(_min(root)); }
    const_iterator end() const { return const_iterator(); }
    // -----------------------------------
  public:
    //-------------------------------
    // the interfaces
    AvlTree() : root(nullptr) {}
    ~AvlTree() { clear(); }
    void clear() { _clear(root); }
    void insert(const T &val) {
        root = _insert(root, val);
        root->parent = nullptr;
    }
    void erase(const T &val) {
        root = _erase(root, val);
        root->parent = nullptr;
    }
    const_iterator find(const T &val) const {
        return const_iterator(_find(root, val));
    }
    inline bool empty() const { return _empty(root); }
    friend std::ostream &operator<<(std::ostream &os, const AvlTree<T> &t) {
        _print(os, t.root, 0);
        return os;
    }
    void swap(AvlTree<T> &rhs) { std::swap(root, rhs.root); }
    friend void swap(AvlTree<T> &lhs, AvlTree<T> &rhs) { lhs.swap(rhs); }
    void merge(AvlTree<T> &rhs) {
        root = _merge(root, rhs.root);
        rhs.root = nullptr;
        root->parent = nullptr;
    }
    //-------------------------------
  private:
    // ---------------------------------
    // some assistant tools
    static Tree _merge(Tree t1, Tree t2) {
        struct Tool {
            typedef Tree List;
            static List cons(Node n, List lst) {
                n->child[1] = lst;
                return n;
            }
            static List copy2list(Tree cur_tree, List cur_lst) {
                if (cur_tree == nullptr)
                    return cur_lst;
                return copy2list(
                    cur_tree->child[0],
                    cons(cur_tree, copy2list(cur_tree->child[1], cur_lst)));
            }
            static List tree2list(Tree t) { return copy2list(t, nullptr); }
            static std::pair<Tree, List> partial(List lst, size_t n) {
                if (n == 0)
                    return std::make_pair(nullptr, lst);
                auto left_n = n / 2;
                auto left_res = partial(lst, left_n);
                Tree left_tree = left_res.first;
                Tree top = left_res.second;
                List left_lst = top->child[1];
                auto right_n = n - 1 - left_n;
                auto right_res = partial(left_lst, right_n);
                Tree right_tree = right_res.first;
                List others = right_res.second;
                top->child[0] = left_tree;
                top->child[1] = right_tree;
                if (left_tree)
                    left_tree->parent = top;
                if (right_tree)
                    right_tree->parent = top;
                update_height(left_tree);
                update_height(right_tree);
                update_height(top);
                return std::make_pair(top, others);
            }
            static Tree list2tree(List lst) {
                size_t len = 0;
                for (Node tmp = lst; tmp; tmp = tmp->child[1], ++len) {
                }
                return partial(lst, len).first;
            }
            static List merge_list(List l1, List l2) {
                Node res = nullptr, n[2] = {l1, l2}, cur = nullptr;
                int dst = (n[0]->val > n[1]->val);
                res = n[dst];
                if (n[0]->val == n[1]->val) {
                    Node tmp = n[dst ^ 1];
                    n[dst ^ 1] = n[dst ^ 1]->child[1];
                    delete tmp;
                }
                n[dst] = n[dst]->child[1];
                cur = res;
                while (n[0] && n[1]) {
                    dst = (n[0]->val > n[1]->val);
                    cur->child[1] = n[dst];
                    cur = n[dst];
                    if (n[0]->val == n[1]->val) {
                        Node tmp = n[dst ^ 1];
                        n[dst ^ 1] = n[dst ^ 1]->child[1];
                        delete tmp;
                    }
                    n[dst] = n[dst]->child[1];
                }
                for (int i = 0; i < 2; ++i) {
                    while (n[i]) {
                        cur->child[1] = n[i];
                        cur = n[i];
                        n[i] = n[i]->child[1];
                    }
                }
                return res;
            }
        };
        return Tool::list2tree(
            Tool::merge_list(Tool::tree2list(t1), Tool::tree2list(t2)));
    }
    static Node _find(Tree t, const T &val) {
        if (t == nullptr)
            return nullptr;
        if (t->val == val)
            return t;
        int rt = (val > t->val);
        return _find(t->child[rt], val);
    }
    static void _print(std::ostream &os, Tree t, int depth) {
        if (t == nullptr)
            return;
        for (int i = 0; i < depth; ++i)
            os << "----";
        os << t->val << std::endl;
        for (int i = 0; i < 2; ++i)
            _print(os, t->child[i], depth + 1);
    }
    static Node make_node(const T &v, Node p = nullptr, Node c1 = nullptr,
                          Node c2 = nullptr, int h = 0) {
        Node res = new ELE(v, c1, c2, p, h);
        return res;
    }
    static inline int height(Tree t) {
        if (t == nullptr)
            return -1;
        return t->height;
    }
    static void update_height(Tree t) {
        if (t == nullptr)
            return;
        t->height = std::max(height(t->child[0]), height(t->child[1])) + 1;
    }
    static inline bool _empty(Tree t) { return t == nullptr; }
    static void _clear(Tree t) {
        if (t == nullptr)
            return;
        for (int i = 0; i < 2; ++i)
            _clear(t->child[i]);
        delete t;
    }
    static Node _min(Tree t) {
        if (t == nullptr)
            return nullptr;
        if (t->child[0])
            return _min(t->child[0]);
        return t;
    }
    static Node _max(Tree t) {
        if (t == nullptr)
            return nullptr;
        if (t->child[1])
            return _max(t->child[1]);
        return t;
    }
    static Tree single_rotate(Tree t, int rt) {
        if (t == nullptr)
            return nullptr;
        Node new_top = t->child[rt];
        Node carry = new_top->child[rt ^ 1];

        new_top->child[rt ^ 1] = t;

        t->child[rt] = carry;

        update_height(t);
        update_height(new_top);

        if (carry)
            carry->parent = t;
        t->parent = new_top;

        return new_top;
    }
    static Tree double_rotate(Tree t, int rt) {
        if (t == nullptr)
            return nullptr;
        Node new_right = t->child[rt];
        Node new_top = new_right->child[rt ^ 1];
        Node new_left = t;
        Node carry1 = new_top->child[rt ^ 1];
        Node carry2 = new_top->child[rt];

        new_left->child[rt] = carry1;
        new_right->child[rt ^ 1] = carry2;
        new_top->child[rt] = new_right;
        new_top->child[rt ^ 1] = new_left;

        update_height(new_left);
        update_height(new_right);
        update_height(new_top);

        new_left->parent = new_right->parent = new_top;
        if (carry1)
            carry1->parent = new_left;
        if (carry2)
            carry2->parent = new_right;

        return new_top;
    }
    static Tree _insert(Tree t, const T &val) {
        if (t == nullptr) { // create a new node
            t = make_node(val);
            return t;
        }
        if (val == t->val)
            return t;

        int rt = (val > t->val);
        t->child[rt] = _insert(t->child[rt], val);
        t->child[rt]->parent = t;
        if (height(t->child[rt]) > height(t->child[rt ^ 1]) + 1) {
            Node tmp = t->child[rt];
            if (height(tmp->child[rt]) > height(tmp->child[rt ^ 1])) {
                t = single_rotate(t, rt);
            } else {
                t = double_rotate(t, rt);
            }
        }
        update_height(t);
        return t;
    }
    static Tree _erase(Tree t, const T &val) {
        if (t == nullptr)
            return nullptr;
        if (val == t->val) {
            for (int i = 0; i < 2; ++i) {
                if (t->child[i] == nullptr) {
                    Node tmp = t;
                    t = t->child[!i];
                    delete tmp;
                    return t;
                }
            }
        }
        int rt = 1; // the branch to be erase
        if (val == t->val) {
            rt = 1;
            Node demn = _min(t->child[1]);
            t->val = demn->val;
            t->child[rt] = _erase(t->child[rt], t->val);
        } else {
            rt = (val > t->val);
            t->child[rt] = _erase(t->child[rt], val);
        }
        if (t->child[rt])
            t->child[rt]->parent = t;

        rt ^= 1;
        if (height(t->child[rt]) > height(t->child[rt ^ 1]) + 1) {
            Node tmp = t->child[rt];
            if (height(tmp->child[rt]) > height(tmp->child[rt ^ 1])) {
                t = single_rotate(t, rt);
            } else {
                t = double_rotate(t, rt);
            }
        }
        update_height(t);
        return t;
    }
    // ---------------------------------------
};

}; // namespace crz

#endif
