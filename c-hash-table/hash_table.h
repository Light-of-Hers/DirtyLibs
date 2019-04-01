#ifndef __CRZ_HASH_TABLE_H__
#define __CRZ_HASH_TABLE_H__

typedef unsigned long long uhash;
typedef unsigned int usize;

typedef void *fctor(void *);
typedef void fdtor(void *);
typedef uhash fhash(void *);
typedef int fcmp(void *, void *);

typedef struct hash_node {
    uhash hashkey;
    void *key, *value;
    struct hash_node *next;
} hash_node;

typedef struct hash_table {
    hash_node **table;
    usize size;
    usize cnt;
    fctor *kctor;
    fctor *vctor;
    fdtor *kdtor;
    fdtor *vdtor;
    fhash *hash;
    fcmp *cmp;
} hash_table;

typedef struct hash_entry {
    char valid;
    void *key;
    void *value;
} hash_entry;

hash_table make_hash_table(fctor *kctor, fctor *vctor, fdtor *kdtor,
                           fdtor *vdtor, fhash *hash, fcmp *cmp);
hash_entry hash_table_read(hash_table *ht, void *key);
void hash_table_write(hash_table *ht, void *key, void *value);
void hash_table_del(hash_table *ht, void *key);
void hash_table_clear(hash_table *ht);

#define HASH_READ(ht, key) hash_table_read(&(ht), (void *)(key))
#define HASH_WRITE(ht, key, value)                                             \
    hash_table_write(&(ht), (void *)(key), (void *)(value))
#define HASH_DEL(ht, key) hash_table_del(&(ht), (void *)(key))
#define HASH_CLEAR(ht) hash_table_clear(&(ht))
#define HASH_FREE(ht) (HASH_CLEAR(ht), free((ht).table))

void *NULL_CTOR(void *x);
void *DEFT_CTOR(void *x);
void *STR_CTOR(void *s);
void DEFT_DTOR(void *x);
void FREE_DTOR(void *x);
uhash DEFT_HASH(void *x);
uhash STR_HASH(void *s);
int STR_CMP(void *a, void *b);

#endif