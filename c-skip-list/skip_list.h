#ifndef __CRZ_SKIP_LIST_H__
#define __CRZ_SKIP_LIST_H__

#define RANDOM_ACCESS

typedef struct skip_node {
    char height;
    int val;
    struct {
#ifdef RANDOM_ACCESS
        int span[2];
#endif
        struct skip_node *ptr[2];
    } * links;
} skip_node;

typedef struct skip_list {
    int size;
    int seed;
    int max_height;
    skip_node *head;
} skip_list;

typedef const skip_node *skip_iter;

skip_list *make_skip_list();
void free_skip_list(skip_list *sl);
skip_iter skip_list_begin(skip_list *sl);
skip_iter skip_list_end(skip_list *sl);

skip_iter skip_list_find(skip_list *sl, int val);
void skip_list_insert(skip_list *sl, int val);
void skip_list_remove(skip_list *sl, int val);
#ifndef RANDOM_ACCESS
void skip_list_rmiter(skip_list *sl, skip_iter);
#endif

skip_iter skip_iter_next(skip_iter iter);
skip_iter skip_iter_prev(skip_iter iter);
int skip_iter_val(skip_iter iter);

#ifdef RANDOM_ACCESS
skip_iter skip_iter_jump(skip_iter iter, int step);
#endif

#endif