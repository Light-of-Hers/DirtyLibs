#include <malloc.h>
#include <stdio.h>

#define elem_t int

typedef struct ELE {
    elem_t val;
    struct ELE *child[2]; // 0: left , 1: right
} * Node, *Tree, *List;
#define next child[1]

//----------------------------
void Insert(Tree *t, elem_t val);
void Delete(Tree *t, elem_t val);
Node Find(Tree t, elem_t val);
Node Max(Tree t);
Node Min(Tree t);
void Rebalance(Tree *t);
//-----------------------------

Node _make_node(elem_t val, Node left, Node right) {
    Node res = (Node)malloc(sizeof(struct ELE));
    res->child[0] = left;
    res->child[1] = right;
    res->val = val;
    return res;
}
Tree _insert(Tree t, elem_t val) {
    if (t == NULL) { return _make_node(val, NULL, NULL); }
    if (val == t->val) return t;
    int isrt = (val > t->val);
    t->child[isrt] = _insert(t->child[isrt], val);
    return t;
}
void Insert(Tree *t, elem_t val) { *t = _insert(*t, val); }

Node Find(Tree t, elem_t val) {
    if (t == NULL) return NULL;
    if (val == t->val) return t;
    int isrt = (val > t->val);
    return Find(t->child[isrt], val);
}

Node Max(Tree t) {
    if (t == NULL) return NULL;
    if (t->child[1]) return Max(t->child[1]);
    return t;
}

Node Min(Tree t) {
    if (t == NULL) return NULL;
    if (t->child[0]) return Min(t->child[0]);
    return t;
}

Tree _delete(Tree t, elem_t val) {
    if (t == NULL) return NULL;
    if (val == t->val) { // Delete t
        Node replace = Min(t->child[1]);
        if (replace) {
            t->val = replace->val;
            t->child[1] = _delete(t->child[1], t->val);
        } else {
            Node erase = t;
            t = t->child[0];
            free(erase);
        }
    } else {
        int isrt = (val > t->val);
        t->child[isrt] = _delete(t->child[isrt], val);
    }
    return t;
}
void Delete(Tree *t, elem_t val) { *t = _delete(*t, val); }

List _cons(Node node, List lst) {
    node->next = lst;
    node->child[0] = NULL;
    return node;
}
static List _copy2list(Tree curtree, List curlist) {
    if (curtree == NULL) return curlist;
    return _copy2list(curtree->child[0],
                      _cons(curtree, _copy2list(curtree->child[1], curlist)));
}
List _tree2list(Tree t) { return _copy2list(t, NULL); }

typedef struct {
    Tree done;
    List left;
} _Pair;
_Pair _make_Pair(Tree a, List b) {
    _Pair res;
    res.done = a;
    res.left = b;
    return res;
}
_Pair _partial(List elts, int size) {
    if (size == 0) { return _make_Pair(NULL, elts); }
    int left_size = size / 2;
    _Pair left_res = _partial(elts, left_size);
    Tree left_tree = left_res.done;
    List left_lst = left_res.left;
    int right_size = size - left_size - 1;
    _Pair right_res = _partial(left_lst->child[1], right_size);
    Tree right_tree = right_res.done;
    List right_lst = right_res.left;
    left_lst->child[0] = left_tree;
    left_lst->child[1] = right_tree;
    return _make_Pair(left_lst, right_lst);
}
Tree _list2tree(List lst) {
    int len = 0;
    Node cur = lst;
    for (; cur; cur = cur->child[1], ++len) {}
    return _partial(lst, len).done;
}

Tree _rebalance(Tree t) {
    List lst = _tree2list(t);
    t = _list2tree(lst);
    return t;
}
void Rebalance(Tree *t) { *t = _rebalance(*t); }

static void _print(Tree t, int depth) {
    if (t == NULL) return;
    for (int i = 0; i < depth; ++i) printf("----");
    printf("%d\n", t->val);
    for (int i = 0; i < 2; ++i) { _print(t->child[i], depth + 1); }
}
void Print(Tree t) { _print(t, 0); }

void Clear(Tree t) {
    if (t == NULL) return;
    Clear(t->child[0]);
    Clear(t->child[1]);
    free(t);
}

int main() {
    Tree t = NULL;
    for (int i = 0; i < 100; ++i) { Insert(&t, i + 1); }
    Rebalance(&t);
    Print(t);
    Clear(t);
}