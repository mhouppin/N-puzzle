#include "heuristic.h"
#include "ft_stdlib.h"

uint64_t manhattan(const NPuzzle *np)
{
    uint64_t ret = 0;

    for (uint16_t sq = 0; sq < np->size * np->size; ++sq)
    {
        if (sq == np->holeIdx)
            continue;
        uint16_t piece = np->board[sq];

        int16_t pf = (int16_t)(piece % np->size);
        int16_t pr = (int16_t)(piece / np->size);
        int16_t sf = (int16_t)(sq % np->size);
        int16_t sr = (int16_t)(sq / np->size);

        ret += (uint64_t)(ft_abs(pf - sf) + ft_abs(pr - sr)) * ONE_MOVE;
    }

    return ret;
}

uint64_t hole_manhattan(const NPuzzle *np)
{
    uint64_t ret = manhattan(np);

    if (np->holeIdx % np->size != 0)
    {
        uint16_t piece = np->board[np->holeIdx - 1];

        if (piece % np->size < (np->holeIdx - 1) % np->size)
            ret -= ONE_MOVE / 4;
    }
    if (np->holeIdx % np->size != np->size - 1)
    {
        uint16_t piece = np->board[np->holeIdx + 1];

        if (piece % np->size > (np->holeIdx + 1) % np->size)
            ret -= ONE_MOVE / 4;
    }
    if (np->holeIdx / np->size != 0)
    {
        uint16_t piece = np->board[np->holeIdx - np->size];

        if (piece / np->size < np->holeIdx / np->size - 1)
            ret -= ONE_MOVE / 4;
    }
    if (np->holeIdx / np->size != np->size - 1)
    {
        uint16_t piece = np->board[np->holeIdx + np->size];

        if (piece / np->size > np->holeIdx / np->size + 1)
            ret -= ONE_MOVE / 4;
    }

    return ret;
}

uint64_t node_value(uint64_t h, uint64_t g, uint64_t weight)
{
    return g * ONE_MOVE + h * weight / ONE_MOVE;
}
