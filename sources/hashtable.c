#include <stdlib.h>
#include "hashtable.h"

int hashtable_init(HashTable *table, size_t size)
{
    table->size = size;
    table->totalNodes = 0;
    table->buckets = calloc(size, sizeof(BucketEntry *));
    if (table->buckets == NULL)
        return -1;

    return 0;
}

int hashtable_insert(HashTable *table, NPuzzle *puzzle)
{
    size_t bucketIdx = puzzle->zobrist % table->size;

    if (table->buckets[bucketIdx] == NULL)
    {
        table->buckets[bucketIdx] = malloc(sizeof(BucketEntry));
        if (table->buckets[bucketIdx] == NULL)
            return -1;

        table->buckets[bucketIdx]->puzzle = puzzle;
        table->buckets[bucketIdx]->next = NULL;
    }
    else
    {
        BucketEntry *entry = table->buckets[bucketIdx];
        while (entry->next != NULL)
            entry = entry->next;

        entry->next = malloc(sizeof(BucketEntry));
        if (entry->next == NULL)
            return -1;

        entry->next->puzzle = puzzle;
        entry->next->next = NULL;
    }

    table->totalNodes++;
    return 0;
}

NPuzzle *hashtable_search(HashTable *table, NPuzzle *puzzle)
{
    size_t bucketIdx = puzzle->zobrist % table->size;

    BucketEntry *entry = table->buckets[bucketIdx];
    while (entry != NULL)
    {
        if (!npuzzle_comp_state(entry->puzzle, puzzle))
            return entry->puzzle;

        entry = entry->next;
    }

    return NULL;
}

void hashtable_destroy(HashTable *table)
{
    for (size_t i = 0; i < table->size; i++)
    {
        BucketEntry *entry = table->buckets[i];
        while (entry != NULL)
        {
            BucketEntry *next = entry->next;
            npuzzle_destroy(entry->puzzle);
            free(entry);
            entry = next;
        }
    }

    free(table->buckets);
}