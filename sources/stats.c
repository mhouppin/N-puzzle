#include "ft_math.h"
#include "ft_stdio.h"
#include "heuristic.h"
#include "stats.h"

clock_t get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void stats_init(Stats *stats, uint64_t weight, bool verbose)
{
    stats->weight = weight;
    stats->verbose = verbose;

    stats->lastPing = 0;
    stats->maxG = 0;
    stats->minH = UINT64_MAX;
    stats->totalNodes = 1;
    stats->totalExplorations = 0;
    stats->multiExplorations = 0;
    stats->pathsShrunk = 0;
    stats->totalPathLength = 0;
    stats->start = get_time_ms();
}

void stats_update_queue_pop(Stats *stats, const NPuzzle *puzzle, size_t bucketSize)
{
    if (stats->maxG < puzzle->g || stats->minH > puzzle->h || stats->lastPing + 50000 <= bucketSize)
    {
        stats->lastPing = bucketSize;
        stats->maxG = ft_maxu(stats->maxG, puzzle->g);
        stats->minH = ft_minu(stats->minH, puzzle->h);

        if (stats->verbose)
        {
            uint64_t v = stats->minH * stats->weight / ONE_MOVE + (ONE_MOVE / 200);
            clock_t s = get_time_ms() - stats->start;
            ft_printf("INFO (%3lu.%03lu seconds, %7zu nodes) G=%lu, H=%lu.%02lu\n",
                s / 1000, s % 1000,
                bucketSize, stats->maxG, v / ONE_MOVE, (v % ONE_MOVE) * 100 / ONE_MOVE);
        }
    }

    stats->totalExplorations++;
}

void stats_update_queue_push(Stats *stats, const NPuzzle *puzzle)
{
    stats->totalNodes++;
    stats->totalPathLength += puzzle->g;
}

void stats_update_duplicate(Stats *stats)
{
    stats->multiExplorations++;
    stats->totalNodes--;
}

void stats_update_shrink(Stats *stats, uint64_t shrinkDistance)
{
    stats->pathsShrunk++;
    stats->totalPathLength -= shrinkDistance;
}

void stats_print(const Stats *stats)
{
    clock_t s = get_time_ms() - stats->start;
    uint64_t avgPathLength100 = stats->totalPathLength * 100 / stats->totalNodes;

    ft_printf("\nSummary:\n");
    ft_printf("  Total time:       %lu.%03lu seconds\n", s / 1000, s % 1000);
    ft_printf("  Total nodes:      %zu\n", stats->totalNodes);
    ft_printf("  Total visits:     %zu\n", stats->totalExplorations);
    ft_printf("  Duplicate nodes:  %zu\n", stats->multiExplorations);
    ft_printf("  Shrunk paths:     %zu\n", stats->pathsShrunk);
    ft_printf("  Avg. path length: %lu.%02lu\n", avgPathLength100 / 100, avgPathLength100 % 100);
}
