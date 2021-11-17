#ifndef STATS_H
# define STATS_H

# include <stdbool.h>
# include <time.h>
# include "npuzzle.h"

typedef struct Stats_
{
    uint64_t weight;
    size_t lastPing;
    uint64_t maxG;
    uint64_t minH;
    size_t totalNodes;
    size_t totalExplorations;
    size_t multiExplorations;
    size_t pathsShrunk;
    uint64_t totalPathLength;
    clock_t start;
    bool verbose;
}
Stats;

void stats_init(Stats *stats, uint64_t weight, bool verbose);
void stats_update_queue_pop(Stats *stats, const NPuzzle *puzzle, size_t bucketSize);
void stats_update_queue_push(Stats *stats, const NPuzzle *puzzle);
void stats_update_duplicate(Stats *stats);
void stats_update_shrink(Stats *stats, uint64_t shrinkDistance);
void stats_print(const Stats *stats);

#endif
