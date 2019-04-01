#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>

#define DEBUG 1

#define doit(x)                                                                \
    if                                                                         \
        constexpr(DEBUG) { x }

template <class T, class Cmp = std::less<T>>
class RbTree {
    typedef std::function<bool(const T &, const T &)> Compare;
    Compare cmp;
    enum Color { RED, BLACK };

    typedef struct ELE {
        T val;
        Color clr;
        ELE *child[2], *parent;
        ELE() : val(), clr(BLACK) { child[0] = child[1] = parent = this; }
        ELE(const T &v, Color c, ELE *l, ELE *r, ELE *p)
            : val(v), clr(c), parent(p) {
            child[0] = l;
            child[1] = r;
        }
    } * Node;
    Node nil;
    Node root;
    size_t siz;

  public:
    RbTree(Compare c = Cmp()) : cmp(c), nil(new ELE()), root(nil), siz(0) {}
    ~RbTree() { clear(); }
    void clear() {
        _clear(root);
        siz = 0;
    }
    bool empty() { return root == nil; }
    size_t size() { return siz; }
    const T &max() { return extreme(root, 1)->val; }
    const T &min() { return extreme(root, 0)->val; }
    void insert(const T &val) { _insert(val); }
    void erase(const T &val) { _erase(_find(val)); }
    void printTree() { _print(root, 0); }

  private:
    Node make_node(const T &val, Color clr = RED) {
        return new ELE(val, clr, nil, nil, nil);
    }
    void _clear(Node t) {
        if (t == nil) return;
        _clear(t->child[0]);
        _clear(t->child[1]);
        delete t;
    }
    void _insert(const T &val) {
        if (root == nil) {
            root = make_node(val, BLACK);
            root->parent = nil;
            return;
        }
        Node cur = root;
        Node p = nil;
        int rt = 0;
        while (cur != nil) {
            p = cur;
            if (cmp(val, cur->val)) {
                cur = cur->child[0];
                rt = 0;
            } else if (cmp(cur->val, val)) {
                cur = cur->child[1];
                rt = 1;
            } else {
                return;
            }
        }
        cur = make_node(val);
        cur->parent = p;
        p->child[rt] = cur;
        _insert_fix(cur);
        siz++;
    }
    void _insert_fix(Node cur) {
        while (cur->parent->clr == RED) {
            int rtc = (cur->parent->child[1] == cur);
            Node p = cur->parent;
            Node g = p->parent;
            int rtp = (p->parent->child[1] == p);
            Node s = p->parent->child[rtp ^ 1];
            if (s->clr == RED) {
                g->clr = RED;
                s->clr = p->clr = BLACK;
                cur = g;
            } else {
                if (rtc != rtp) {
                    rotate(p, rtc);
                    std::swap(cur, p);
                }
                rotate(g, rtp);
                g->clr = RED;
                p->clr = BLACK;
                cur = p;
            }
        }
        if (cur->parent == nil) root = cur;
        root->clr = BLACK;
    }
    Node _find(const T &val) {
        Node cur = root;
        while (cur != nil) {
            if (cmp(val, cur->val)) {
                cur = cur->child[0];
            } else if (cmp(cur->val, val)) {
                cur = cur->child[1];
            } else {
                break;
            }
        }
        return cur;
    }
    void _erase(Node del) {
        if (del == nil) return;
        Color org_clr = del->clr;
        Node sub = nil;
        Node succ = nil;
        if (del->child[1] == nil) {
            sub = del->child[0];
            succ = sub;
        } else if (del->child[0] == nil) {
            sub = del->child[1];
            succ = sub;
        } else {
            sub = extreme(del->child[1], 0);
            org_clr = sub->clr;
            succ = sub->child[1];
            transplant(sub, succ);
            sub->child[1] = del->child[1];
            sub->child[1]->parent = sub;
            sub->child[0] = del->child[0];
            sub->child[0]->parent = sub;
            sub->clr = del->clr;
        }
        transplant(del, sub);
        delete del;
        if (org_clr == BLACK) _erase_fix(succ);
        siz--;
    }
    void _erase_fix(Node cur) {
        while (cur->clr == BLACK && cur != root) {
            Node p = cur->parent;
            int rt = p->child[1] == cur;
            Node b = p->child[rt ^ 1];
            if (b->clr == RED) {
                std::swap(b->clr, p->clr);
                rotate(p, rt ^ 1);
                b = p->child[rt ^ 1];
            }
            if (b->child[0]->clr == BLACK && b->child[1]->clr == BLACK) {
                b->clr = RED;
                cur = p;
            } else {
                if (b->child[rt ^ 1]->clr == BLACK) {
                    std::swap(b->child[rt]->clr, b->clr);
                    rotate(b, rt);
                    b = b->parent;
                }
                std::swap(b->clr, p->clr);
                b->child[rt ^ 1]->clr = BLACK;
                rotate(p, rt ^ 1);
                cur = root;
            }
        }
        cur->clr = BLACK;
    }
    Node extreme(Node n, int rt) {
        if (n == nil) return n;
        Node cur = n;
        while (1) {
            if (cur->child[rt] == nil) return cur;
            cur = cur->child[rt];
        }
    }
    void rotate(Node n, int rt) {
        int rtp = n->parent->child[1] == n;

        Node top = n->child[rt];
        Node carry = top->child[rt ^ 1];

        top->parent = n->parent;
        top->parent->child[rtp] = top;

        n->parent = top;
        n->parent->child[rt ^ 1] = n;

        carry->parent = n;
        carry->parent->child[rt] = carry;

        if (n == root) root = top;
    }
    void transplant(Node old, Node now) {
        if (old == root) {
            root = now;
        } else {
            int rt = (old->parent->child[1] == old);
            old->parent->child[rt] = now;
        }
        now->parent = old->parent;
    }
    void _print(Node t, int depth) {
        if (t == nil) return;
        for (int i = 0; i < depth; ++i) std::cout << "----";
        std::cout << t->val << std::endl;
        _print(t->child[0], depth + 1);
        _print(t->child[1], depth + 1);
    }
};

int main() {
    RbTree<int> t;
    for (int i = 100; i > 0; --i) t.insert(i);
    for (int i = 100; i > 50; --i) t.erase(i);
    t.printTree();
}