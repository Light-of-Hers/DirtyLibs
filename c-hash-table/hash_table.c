#include "hash_table.h"
#include <malloc.h>
#include <string.h>

#define for_each_node(ht, idx, nd)                                             \
    for (hash_node *nd = ht->table[idx]; nd; nd = nd->next)

static hash_node *make_hash_node(uhash hashkey, void *key, void *value) {
    hash_node *nd = malloc(sizeof(hash_node));
    nd->hashkey = hashkey, nd->key = key, nd->value = value;
    nd->next = NULL;
    return nd;
}

static const usize init_size = (1 << 8);
hash_table make_hash_table(fctor *kctor, fctor *vctor, fdtor *kdtor,
                           fdtor *vdtor, fhash *hash, fcmp *cmp) {
    hash_table ht = {calloc(sizeof(hash_node *), init_size),
                     init_size,
                     0,
                     kctor,
                     vctor,
                     kdtor,
                     vdtor,
                     hash,
                     cmp};
    return ht;
}

static void rehash(hash_table *ht) {
    usize oldsize = ht->size;
    usize newsize = oldsize;
    if (ht->cnt >= oldsize) {
        newsize <<= 1;
    } else if (ht->cnt * 10 < oldsize && oldsize > init_size) {
        newsize >>= 1;
    }
    if (newsize == oldsize)
        return;

    hash_node **table = calloc(sizeof(hash_node *), newsize);
    for (int i = 0; i < oldsize; ++i) {
        while (ht->table[i]) {
            hash_node *nd = ht->table[i];
            usize idx = nd->hashkey & (newsize - 1);
            ht->table[i] = nd->next;
            nd->next = table[idx];
            table[idx] = nd;
        }
    }
    free(ht->table);
    ht->table = table;
    ht->size = newsize;
}

hash_entry hash_table_read(hash_table *ht, void *key) {
    uhash hk = ht->hash(key);
    usize idx = hk & (ht->size - 1);
    hash_entry he = {0, 0, 0};
    for_each_node(ht, idx, nd) {
        if (hk == nd->hashkey && ht->cmp(key, nd->key) == 0) {
            he.key = nd->key, he.value = nd->value, he.valid = 1;
            break;
        }
    }
    return he;
}

void hash_table_write(hash_table *ht, void *key, void *value) {
    uhash hk = ht->hash(key);
    usize idx = hk & (ht->size - 1);
    for_each_node(ht, idx, nd) {
        if (hk == nd->hashkey && ht->cmp(key, nd->key) == 0) {
            ht->vdtor(nd->value);
            nd->value = ht->vctor(value);
            return;
        }
    }
    hash_node *nd = make_hash_node(hk, ht->kctor(key), ht->vctor(value));
    nd->next = ht->table[idx];
    ht->table[idx] = nd;
    ht->cnt++;
    rehash(ht);
}

void hash_table_del(hash_table *ht, void *key) {
    uhash hk = ht->hash(key);
    usize idx = hk & (ht->size - 1);
    hash_node *cur = ht->table[idx], *pre = NULL;
    while (cur) {
        if (hk == cur->hashkey && ht->cmp(key, cur->key) == 0) {
            if (pre == NULL) {
                ht->table[idx] = cur->next;
            } else {
                pre->next = cur->next;
            }
            ht->kdtor(cur->key), ht->vdtor(cur->value);
            free(cur);
            ht->cnt--;
            rehash(ht);
            return;
        }
        pre = cur, cur = cur->next;
    }
}

void hash_table_clear(hash_table *ht) {
    for (int i = 0; i < ht->size; ++i) {
        hash_node *nd = ht->table[i];
        while (nd) {
            ht->kdtor(nd->key), ht->vdtor(nd->value);
            hash_node *tmp = nd;
            nd = nd->next;
            free(tmp);
        }
        ht->table[i] = NULL;
    }
    ht->cnt = 0;
}

void *NULL_CTOR(void *x) {
    return NULL;
}
void *DEFT_CTOR(void *x) {
    return x;
}
void *STR_CTOR(void *s) {
    char *res = calloc(sizeof(char), strlen((char *)s) + 1);
    strcpy(res, (char *)s);
    return res;
}
void DEFT_DTOR(void *x) {
    return;
}
void FREE_DTOR(void *x) {
    free(x);
}
uhash DEFT_HASH(void *x) {
    const static uhash factor = 100000007;
    return (uhash)x * factor;
}
uhash STR_HASH(void *s) {
    const static uhash base = 100000007;
    uhash res = 0;
    for (char *p = s; *p; ++p)
        res = (res + (uhash)*p) * base;
    return res;
}
int STR_CMP(void *a, void *b) {
    return strcmp(a, b);
}
