#include "astar.h"
#include "ft_heap.h"
#include "ft_prior_queue.h"
#include "ft_stdio.h"
#include "ft_stdlib.h"
#include "ft_vector.h"

void dump_queue(PriorQueue *pq)
{
    extern uint64_t Weight;

    for (size_t i = 0; i < pq->vec.itemCount; i++)
    {
        NPuzzle *np = *(NPuzzle **)vector_at(&pq->vec, i);
        ft_printf("Node 0x%08x: G(%lu), H(%lu), F(%lu)\n", (unsigned int)np->zobrist,
            np->g, np->h, node_value(np->h, np->g, Weight));
    }
}

void push_npuzzle(PriorQueue *pq, Vector *stateBucket, NPuzzle *base, uint16_t sq, heuristic_t h)
{
    NPuzzle *next = npuzzle_dup(base);

    npuzzle_apply(next, sq);

    // Check if the puzzle is already in the state bucket
    NPuzzle **data = vector_search(stateBucket, &next, npuzzle_comp_state);

    if (data)
    {
        NPuzzle *bucketData = *data;

        // If it is, update the bucket data if the new one has a shorter path from start
        // Note: next->parent could be replaced by base there
        if (bucketData->g > next->g)
        {
            ft_printf("Shrink path for node 0x%08x: %lu -> %lu\n", (unsigned int)bucketData->zobrist, bucketData->g, next->g);
            ft_printf("Shrink after %lu nodes\n", stateBucket->itemCount);

            bucketData->g = next->g;
            bucketData->parent = next->parent;

            NPuzzle **ptr = ft_lsearch(&bucketData, pq->vec.data, pq->vec.itemCount, sizeof(NPuzzle *), npuzzle_comp_state);

            // Push bucketData on the queue again if it is not there yet, since we have a new shorter path
            if (!ptr)
            {
                ft_printf("Push shrunk node to queue\n");
                pqueue_push(pq, &bucketData);
                // dump_queue(pq);
                ft_printf("\n");
            }
            else
            {
                ft_printf("Update shrunk node position into queue\n");
                // heap_adjust(pq->vec.data, pq->vec.itemSize, pq->vec.itemCount, npuzzle_comp_value, ptr - (NPuzzle **)pq->vec.data);
                heap_make(pq->vec.data, pq->vec.itemSize, pq->vec.itemCount, npuzzle_comp_value);
            }
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

void launch_astar(NPuzzle *np, heuristic_t h)
{
    extern uint64_t Weight;
    PriorQueue pq;
    Vector stateBucket;

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
        if (vector_size(&pq.vec) > 100000000)
        {
            pqueue_destroy(&pq);
            vector_destroy(&stateBucket);
            return ;
        }

        NPuzzle *npCur;

        pqueue_pop(&pq, &npCur);

        size_t fuckedNode = heap_is_ok_until(pq.vec.data, pq.vec.itemSize, pq.vec.itemCount, npuzzle_comp_value);

        if (fuckedNode != pq.vec.itemCount)
        {
            ft_printf("Heap is fucked after %lu nodes\n", stateBucket.itemCount);
            // dump_queue(&pq);
            return ;
        }

        if (npuzzle_solved(npCur))
        {
            ft_printf("Path length: %lu\n", (unsigned long)npCur->g);

            // TODO: reconstruct the path

            pqueue_destroy(&pq);
            vector_destroy(&stateBucket);
            return ;
        }

        // Check which moves are possible and add the reached states to the queue
        uint16_t holeIdx = npCur->holeIdx;
        uint16_t size = npCur->size;

        if (holeIdx % size != 0)
            push_npuzzle(&pq, &stateBucket, npCur, holeIdx - 1, h);
        if (holeIdx % size != size - 1)
            push_npuzzle(&pq, &stateBucket, npCur, holeIdx + 1, h);
        if (holeIdx / size != 0)
            push_npuzzle(&pq, &stateBucket, npCur, holeIdx - size, h);
        if (holeIdx / size != size - 1)
            push_npuzzle(&pq, &stateBucket, npCur, holeIdx + size, h);

        // dump_queue(&pq);
        // ft_printf("\n");
    }
}
