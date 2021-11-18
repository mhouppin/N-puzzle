#ifndef HASHTABLE_H
# define HASHTABLE_H

# include "npuzzle.h"

typedef struct BucketEntry_
{
    NPuzzle *puzzle;
    struct BucketEntry_ *next;
}
BucketEntry;

typedef struct HashTable_
{
    size_t size;
    BucketEntry **buckets;
    size_t totalNodes;
}
HashTable;

int hashtable_init(HashTable *table, size_t size);
int hashtable_insert(HashTable *table, NPuzzle *puzzle);
NPuzzle *hashtable_search(HashTable *table, NPuzzle *puzzle);
void hashtable_destroy(HashTable *table);

# endif