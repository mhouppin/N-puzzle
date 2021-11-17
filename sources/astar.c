#include "astar.h"
#include "ft_heap.h"
#include "ft_math.h"
#include "ft_prior_queue.h"
#include "ft_stdio.h"
#include "ft_stdlib.h"
#include "ft_vector.h"
#include "stats.h"

void backprop_path(NPuzzle *np, uint64_t depth, bool first)
{
    if (np->parent)
        backprop_path(np->parent, depth - 1, false);

    ft_printf("%*hu%s", (np->size > 31) + (np->size > 9) + (np->size > 3) + 1, np->holeIdx,
        first || (depth % 10 == 9) ? "\n" : " - ");
}

void push_npuzzle(PriorQueue *pq, Vector *stateBucket, NPuzzle *base, uint16_t sq, heuristic_t h, Stats *stats)
{
    NPuzzle *next = npuzzle_dup(base);

    npuzzle_apply(next, sq);
    stats_update_queue_push(stats, next);

    // Check if the puzzle is already in the state bucket
    NPuzzle **data = vector_search(stateBucket, &next, npuzzle_comp_state);

    if (data)
    {
        NPuzzle *bucketData = *data;

        stats_update_duplicate(stats);

        // If it is, update the bucket data if the new one has a shorter path from start
        // Note: next->parent could be replaced by base there
        if (bucketData->g > next->g)
        {
            stats_update_shrink(stats, bucketData->g - next->g);
            bucketData->g = next->g;
            bucketData->parent = next->parent;

            NPuzzle **ptr = ft_lsearch(&bucketData, pq->vec.data, pq->vec.itemCount, sizeof(NPuzzle *), npuzzle_comp_state);

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
        vector_insert(stateBucket, vector_index(stateBucket, &next, npuzzle_comp_state), &next);
    }
}

void launch_astar(NPuzzle *np, heuristic_t h, size_t maxNodes, bool verbose)
{
    extern uint64_t Weight;
    PriorQueue pq;
    Vector stateBucket;
    Stats stats;

    stats_init(&stats, Weight, verbose);
    pqueue_init(&pq, sizeof(NPuzzle *), &npuzzle_comp_value);
    vector_init(&stateBucket, sizeof(NPuzzle *));
    vector_set_item_dtor(&stateBucket, &untyped_npuzzle_destroy);

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

        if (vector_push_back(&stateBucket, &npDup))
        {
            npuzzle_destroy(npDup);
            pqueue_destroy(&pq);
            return ;
        }
    }

    while (!pqueue_empty(&pq))
    {
        // Simple security to start since I don't want to generate an OOM
        if (vector_size(&pq.vec) >= maxNodes)
        {
            ft_printf("Stopping search, max nodes reached.\n");
            stats_print(&stats);

            pqueue_destroy(&pq);
            vector_destroy(&stateBucket);
            return ;
        }

        NPuzzle *npCur;

        pqueue_pop(&pq, &npCur);

        stats_update_queue_pop(&stats, npCur, vector_size(&pq.vec));

        if (npuzzle_solved(npCur))
        {
            ft_printf("\nPath length: %lu\nHole squares:\n", (unsigned long)npCur->g);
            backprop_path(npCur, npCur->g, true);
            stats_print(&stats);

            pqueue_destroy(&pq);
            vector_destroy(&stateBucket);
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

    ft_printf("Puzzle unsolvable.\n");
    stats_print(&stats);

    pqueue_destroy(&pq);
    vector_destroy(&stateBucket);
}
