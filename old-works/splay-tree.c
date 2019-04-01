#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int val;
    struct Node *cd[2], *pt;
} Node, *node_t;
Node NILL;
node_t nil, root;
inline int is_rt(node_t nd) {
    return nd->pt->cd[1] == nd;
}
node_t make_node(int val) {
    node_t nd = malloc(sizeof(Node));
    nd->val = val;
    nd->pt = nd->cd[0] = nd->cd[1] = nil;
    return nd;
}
void print_tree(node_t nd, int depth);
void init() {
    nil = &NILL;
    nil->cd[0] = nil->cd[1] = nil->pt = nil;
    root = nil;
}
void rotate(node_t nd, int rt) {
    node_t cd = nd->cd[rt ^ 1];
    node_t pt = nd->pt;
    node_t gd = pt->pt;
    gd->cd[is_rt(pt)] = nd;
    pt->cd[rt] = cd;
    nd->cd[rt ^ 1] = pt;
    nd->pt = gd;
    pt->pt = nd;
    cd->pt = pt;
}

node_t splay(node_t nd) {
    node_t pt, gd;
    int rt;
    while (1) {
        if (nd->pt == nil)
            break;
        pt = nd->pt;
        rt = is_rt(nd);
        if (pt->pt == nil) {
            rotate(nd, rt);
            break;
        }
        gd = pt->pt;
        if (rt == is_rt(pt)) {
            rotate(pt, rt);
            rotate(nd, rt);
        } else {
            rotate(nd, rt);
            rotate(nd, rt ^ 1);
        }
    }
    return nd;
}

node_t insert(node_t nd, node_t beg) {
    node_t pre, cur;
    int rt;
    if (beg == nil)
        return nd;
    cur = beg, pre = nil, rt = 0;
    while (1) {
        if (cur == nil) {
            pre->cd[rt] = nd;
            nd->pt = pre;
            break;
        }
        if (cur->val == nd->val)
            break;
        rt = cur->val < nd->val;
        pre = cur;
        cur = pre->cd[rt];
    }
    return splay(nd);
}
int find(int val, node_t *beg) {
    node_t cur;
    int rt;
    if (*beg == nil)
        return 0;
    cur = *beg, rt = 0;
    while (1) {
        if (cur == nil)
            return 0;
        if (cur->val == val)
            break;
        rt = cur->val < val;
        cur = cur->cd[rt];
    }
    *beg = splay(cur);
    return 1;
}
inline node_t plant(node_t nd, node_t pt, int rt) {
    nd->pt = pt;
    pt->cd[rt] = nd;
    return nd;
}
node_t minnd(node_t beg) {
    node_t cur;
    if (beg == nil)
        return nil;
    cur = beg;
    while (cur->cd[0] != nil)
        cur = cur->cd[0];
    return cur;
}

node_t erase(node_t nd) {
    if (nd->cd[0] == nil) {
        return splay(plant(nd->cd[1], nd->pt, is_rt(nd)));
    } else if (nd->cd[1] == nil) {
        return splay(plant(nd->cd[0], nd->pt, is_rt(nd)));
    } else {
        node_t sub = minnd(nd->cd[1]);
        if (sub->pt == nd) {
            plant(nd->cd[0], sub, 0);
        } else {
            plant(sub->cd[1], sub->pt, is_rt(sub));
            plant(nd->cd[0], sub, 0);
            plant(nd->cd[1], sub, 1);
        }
        return splay(plant(sub, nd->pt, is_rt(nd)));
    }
}

void print_tree(node_t nd, int depth) {
    if (nd == nil)
        return;
    for (int i = 0; i < depth; ++i)
        printf("--");
    printf("%d\n", nd->val);
    print_tree(nd->cd[0], depth + 1);
    print_tree(nd->cd[1], depth + 1);
}
void check(node_t nd, node_t pre) {
    if (nd == nil)
        return;
    if (nd->pt != pre)
        printf("fuck: %d\n", nd->val);
    check(nd->cd[0], nd);
    check(nd->cd[1], nd);
}

int main() {
    init();
    for (int i = 0; i < 100; ++i) {
        root = insert(make_node(i), root);
    }
    print_tree(root, 0);
    // for (int i = 19; i >= 0; --i) {
    //     find(i, &root);
    // }
    // for (int i = 0; i < 20; ++i) {
    //     find(i, &root);
    // }
    for (int i = 0; i < 100; i++) {
        find(rand() % 100, &root);
    }
    // find(50, &root);
    print_tree(root, 0);
}