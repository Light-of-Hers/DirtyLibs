#include "skip_list.h"
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#define MAX_HEIGHT 32

skip_list *make_skip_list() {
    skip_list *sl = malloc(sizeof(skip_list));
    sl->size = 0;
    sl->seed = time(NULL);
    sl->head = malloc(sizeof(skip_node));
    skip_node *hd = sl->head;
    hd->height = MAX_HEIGHT;
    hd->links = calloc(sizeof(*hd->links), MAX_HEIGHT);
    for (int i = 0; i < MAX_HEIGHT; ++i) {
        hd->links[i].ptr[0] = hd->links[i].ptr[1] = hd;
#ifdef RANDOM_ACCESS
        hd->links[i].span[0] = hd->links[i].span[1] = 1;
#endif
    }
    return sl;
}

static skip_node *make_skip_node(skip_list *sl, int val) {
    srand(sl->seed), sl->seed = rand();
    int h = 1, rm = RAND_MAX >> 1;
    while (rm > sl->seed)
        rm >>= 1, h++;
    skip_node *nd = malloc(sizeof(skip_node));
    nd->height = h;
    nd->val = val;
    nd->links = calloc(sizeof(*nd->links), nd->height);
    return nd;
}

static void free_skip_node(skip_node *nd) {
    free(nd->links), free(nd);
}

void free_skip_list(skip_list *sl) {
    skip_node *cur = sl->head->links[0].ptr[1];
    while (cur != sl->head) {
        skip_node *tmp = cur;
        cur = cur->links[0].ptr[1];
        free_skip_node(tmp);
    }
    free_skip_node(sl->head), free(sl);
}

skip_iter skip_list_find(skip_list *sl, int val) {
    int h = sl->head->height - 1;
    skip_node *cur = sl->head;
    while (h >= 0) {
        skip_node *fwd = cur->links[h].ptr[1];
        if (fwd == sl->head || fwd->val > val) {
            h--;
        } else if (fwd->val < val) {
            cur = fwd;
        } else {
            return fwd;
        }
    }
    return sl->head;
}

void skip_list_insert(skip_list *sl, int val) {
    skip_node *record[MAX_HEIGHT] = {0};
#ifdef RANDOM_ACCESS
    int start[MAX_HEIGHT] = {0};
    int step = 0;
#endif
    int h = sl->head->height - 1;
    skip_node *cur = sl->head;
    while (h >= 0) {
        skip_node *fwd = cur->links[h].ptr[1];
        if (fwd == sl->head || fwd->val > val) {
#ifdef RANDOM_ACCESS
            start[h] = step;
#endif
            record[h--] = cur;
        } else if (fwd->val < val) {
#ifdef RANDOM_ACCESS
            step += cur->links[h].span[1];
#endif
            cur = fwd;
        } else {
            return;
        }
    }
    skip_node *new = make_skip_node(sl, val);
    for (int i = 0; i < new->height; ++i) {
        skip_node *next = record[i]->links[i].ptr[1];
        skip_node *prev = record[i];
        new->links[i].ptr[1] = next, next->links[i].ptr[0] = new;
        new->links[i].ptr[0] = prev, prev->links[i].ptr[1] = new;
#ifdef RANDOM_ACCESS
        new->links[i].span[1] = prev->links[i].span[1] - (step - start[i]);
        prev->links[i].span[1] = step + 1 - start[i];
        new->links[i].span[0] = prev->links[i].span[1];
        next->links[i].span[0] = new->links[i].span[1];
#endif
    }
#ifdef RANDOM_ACCESS
    for (int i = new->height; i < MAX_HEIGHT; ++i) {
        record[i]->links[i].span[1] += 1;
        record[i]->links[i].ptr[1]->links[i].span[0] += 1;
    }
#endif
    sl->size++;
}

void skip_list_remove(skip_list *sl, int val) {
    skip_node *record[MAX_HEIGHT] = {0};

    int h = sl->head->height - 1;
    skip_node *cur = sl->head;
    while (h >= 0) {
        skip_node *fwd = cur->links[h].ptr[1];
        if (fwd == sl->head || fwd->val > val) {
            record[h--] = cur;
        } else if (fwd->val < val) {
            cur = fwd;
        } else {
            for (int i = 0; i < fwd->height; ++i) {
                skip_node *next = fwd->links[i].ptr[1];
                skip_node *prev = fwd->links[i].ptr[0];
                prev->links[i].ptr[1] = next;
                next->links[i].ptr[0] = prev;
#ifdef RANDOM_ACCESS
                prev->links[i].span[1] = next->links[i].span[0] =
                    fwd->links[i].span[1] + fwd->links[i].span[0] - 1;
#endif
            }
#ifdef RANDOM_ACCESS
            for (int i = fwd->height; i < MAX_HEIGHT; ++i) {
                record[i]->links[i].span[1] -= 1;
                record[i]->links[i].ptr[1]->links[i].span[0] -= 1;
            }
#endif
            free_skip_node(fwd);
            sl->size--;
            return;
        }
    }
}

#ifndef RANDOM_ACCESS
void skip_list_rmiter(skip_list *sl, skip_iter iter) {
    assert(iter != skip_list_end(sl));
    skip_node *cur = (skip_node *)iter;
    for (int i = 0; i < cur->height; ++i) {
        skip_node *prev = cur->links[i].ptr[0];
        skip_node *next = cur->links[i].ptr[1];
        prev->links[i].ptr[1] = next, next->links[i].ptr[0] = prev;
    }
    free_skip_node(cur);
    sl->size--;
}
#endif

skip_iter skip_list_begin(skip_list *sl) {
    return sl->head->links[0].ptr[1];
}
skip_iter skip_list_end(skip_list *sl) {
    return sl->head;
}

skip_iter skip_iter_next(skip_iter iter) {
    return iter->links[0].ptr[1];
}

skip_iter skip_iter_prev(skip_iter iter) {
    return iter->links[0].ptr[0];
}

int skip_iter_val(skip_iter iter) {
    return iter->val;
}

#ifdef RANDOM_ACCESS
skip_iter skip_iter_jump(skip_iter iter, int step) {
    if (step == 0)
        return iter;
    int fwd = step > 0;
    step = fwd ? step : -step;

    int h = iter->height - 1;
    const skip_node *cur = iter;
    while (step > 0) {
        int span = cur->links[h].span[fwd];
        if (span <= step) {
            step -= span;
            cur = cur->links[h].ptr[fwd];
        } else {
            h--;
        }
    }
    return cur;
}
#endif