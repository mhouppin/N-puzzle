#include "astar.h"
#include "hashtable.h"
#include "ft_heap.h"
#include "ft_math.h"
#include "ft_prior_queue.h"
#include "ft_stdio.h"
#include "ft_stdlib.h"
#include "stats.h"

void backprop_path(NPuzzle *np, uint64_t depth, bool first)
{
    if (np->parent)
        backprop_path(np->parent, depth - 1, false);
    else
        return;

    // Use the difference between the previous hole position and the current one
    // to determine the direction of the move.

    int16_t diff = (int16_t)(np->parent->holeIdx - np->holeIdx);

    const char *direction = diff == -1 ? "left"
                          : diff ==  1 ? "right"
                          : diff == (int16_t)(uint16_t)np->size ? "down" : "up";

    ft_printf("%5s%s", direction, first || (depth % 10 == 0) ? "\n" : " - ");
}

void push_npuzzle(PriorQueue *pq, HashTable *stateBucket, NPuzzle *base, uint16_t sq, heuristic_t h, Stats *stats)
{
    NPuzzle *next = npuzzle_dup(base);

    npuzzle_apply(next, sq);
    stats_update_queue_push(stats, next);

    // Check if the puzzle is already in the state bucket
    NPuzzle *bucketData = hashtable_search(stateBucket, next);

    if (bucketData)
    {
        stats_update_duplicate(stats);

        // If it is, update the bucket data if the new one has a shorter path from start
        // Note: next->parent could be replaced by base there
        if (bucketData->g > next->g)
        {
            stats_update_shrink(stats, bucketData->g - next->g);
            bucketData->g = next->g;
            bucketData->parent = next->parent;

            NPuzzle **ptr = ft_lsearch(&bucketData, pq->vec.data, pq->vec.itemCount, sizeof(NPuzzle *), npuzzle_comp_stateptr);

            // Push bucketData on the queue again if it is not there yet, since we have a new shorter path
            if (!ptr)
                pqueue_push(pq, &bucketData);
            else
                heap_push(pq->vec.data, pq->vec.itemSize, (size_t)(ptr - (NPuzzle **)pq->vec.data) + 1, npuzzle_comp_value);
        }

        // Free next, since we already have it in the state bucket
        npuzzle_destroy(next);
    }
    // If it isn't, push it in both the state bucket and the queue
    else
    {
        next->h = h(next);
        pqueue_push(pq, &next);
        hashtable_insert(stateBucket, next);
    }
}

void launch_astar(NPuzzle *np, heuristic_t h, size_t maxNodes, bool verbose)
{
    extern uint64_t Weight;
    PriorQueue pq;
    HashTable stateBucket;
    Stats stats;

    stats_init(&stats, Weight, verbose);
    pqueue_init(&pq, sizeof(NPuzzle *), &npuzzle_comp_value);

    if (hashtable_init(&stateBucket, maxNodes))
    {
        ft_dprintf(2, "Error: Could not allocate memory for the state bucket\n");
        return ;
    }

    if (verbose)
        ft_printf("Setting upper limit for nodes to %zu.\n", maxNodes);

    // Add the initial state to the queue and the bucket
    {
        NPuzzle *npDup = npuzzle_dup(np);
        if (npDup == NULL)
            return ;
        npDup->parent = NULL;
        npDup->h = h(npDup);

        if (pqueue_push(&pq, &npDup))
        {
            npuzzle_destroy(npDup);
            return ;
        }

        if (hashtable_insert(&stateBucket, npDup))
        {
            ft_dprintf(2, "Error: Could not insert the initial state in the state bucket\n");
            npuzzle_destroy(npDup);
            pqueue_destroy(&pq);
            return ;
        }
    }

    while (!pqueue_empty(&pq))
    {
        // Check if we're running out of nodes/memory on our search
        if (stateBucket.totalNodes >= maxNodes)
        {
            ft_printf("Stopping search, max nodes reached.\n");
            stats_print(&stats);

            pqueue_destroy(&pq);
            hashtable_destroy(&stateBucket);
            return ;
        }

        NPuzzle *npCur;

        // Get the top puzzle from the priority queue and update search stats
        pqueue_pop(&pq, &npCur);
        stats_update_queue_pop(&stats, npCur, stateBucket.totalNodes);

        // Check if we just completed the puzzle
        if (npuzzle_solved(npCur))
        {
            ft_printf("\nPath length: %lu\nHole movements:\n", (unsigned long)npCur->g);
            backprop_path(npCur, npCur->g, true);
            stats_print(&stats);

            pqueue_destroy(&pq);
            hashtable_destroy(&stateBucket);
            return ;
        }

        // Check which moves are possible and add the reached states to the queue
        uint16_t holeIdx = npCur->holeIdx;
        uint16_t size = npCur->size;

        if (holeIdx % size != 0)
            push_npuzzle(&pq, &stateBucket, npCur, holeIdx - 1, h, &stats);
        if (holeIdx % size != size - 1)
            push_npuzzle(&pq, &stateBucket, npCur, holeIdx + 1, h, &stats);
        if (holeIdx / size != 0)
            push_npuzzle(&pq, &stateBucket, npCur, holeIdx - size, h, &stats);
        if (holeIdx / size != size - 1)
            push_npuzzle(&pq, &stateBucket, npCur, holeIdx + size, h, &stats);
    }

    // We should not arrive here since solvability detection is done above

    ft_dprintf(2, "Error: puzzle is not solvable (failed detection?)\n");
    stats_print(&stats);

    pqueue_destroy(&pq);
    hashtable_destroy(&stateBucket);
}
