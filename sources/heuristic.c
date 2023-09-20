#include <stdbool.h>
#include <stdio.h>
#include "heuristic.h"
#include "ft_stdlib.h"

static int16_t manhattan_dist(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    return (ft_abs(x1 - x2) + ft_abs(y1 - y2));
}

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

        ret += (uint64_t)manhattan_dist(pf, pr, sf, sr) * ONE_MOVE;
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
            return ret;
    }
    if (np->holeIdx % np->size != np->size - 1)
    {
        uint16_t piece = np->board[np->holeIdx + 1];

        if (piece % np->size > (np->holeIdx + 1) % np->size)
            return ret;
    }
    if (np->holeIdx / np->size != 0)
    {
        uint16_t piece = np->board[np->holeIdx - np->size];

        if (piece / np->size < np->holeIdx / np->size - 1)
            return ret;
    }
    if (np->holeIdx / np->size != np->size - 1)
    {
        uint16_t piece = np->board[np->holeIdx + np->size];

        if (piece / np->size > np->holeIdx / np->size + 1)
            return ret;
    }

    return ret + 2 * ONE_MOVE;
}

// Checks if moving a piece acording to dirX and dirY will make it closer to its goal position.
bool shortens_path(const NPuzzle *np, int16_t x, int16_t y, int16_t dirX, int16_t dirY, int16_t s)
{
    uint16_t piece = np->board[y * s + x];
    int16_t targetX = (int16_t)piece % s;
    int16_t targetY = (int16_t)piece / s;

    return manhattan_dist(x, y, targetX, targetY) > manhattan_dist(x + dirX, y + dirY, targetX, targetY);
}

uint64_t bwp_manhattan(const NPuzzle *np)
{
    uint64_t ret = manhattan(np);
    size_t dist = 0;

    // If the puzzle is already solved, we can stop here.
    if (ret == 0)
        return 0;

    const int16_t holeX = (int16_t)(np->holeIdx % np->size);
    const int16_t holeY = (int16_t)(np->holeIdx / np->size);
    const int16_t s = (int16_t)np->size;

    while (dist < np->size)
    {
        int16_t d = (int16_t)(dist + 1), shift;

        // Check if we can use the hole to improve a piece position in a square radius of dist.
        // If that's not possible, increase dist until we can.

        // Check the top corners of the diamond.
        if (holeX + d < (int16_t)np->size && shortens_path(np, holeX + d, holeY, -1, 0, s))
            break ;

        if (holeY + d < (int16_t)np->size && shortens_path(np, holeX, holeY + d, 0, -1, s))
            break ;

        if (holeX - d >= 0 && shortens_path(np, holeX - d, holeY, 1, 0, s))
            break ;

        if (holeY - d >= 0 && shortens_path(np, holeX, holeY - d, 0, 1, s))
            break ;

        // Check the sides of the diamond.
        for (shift = 1; shift < d; ++shift)
        {
            int16_t x = holeX + shift;
            int16_t y = holeY + d - shift;

            // Check that the targeted square is valid.
            if (x >= 0 && x < s && y >= 0 && y < s)
                if (shortens_path(np, x, y, -1, 0, s) || shortens_path(np, x, y, 0, -1, s))
                    break ;

            x = holeX - shift;
            y = holeY + d - shift;

            if (x >= 0 && x < s && y >= 0 && y < s)
                if (shortens_path(np, x, y, 1, 0, s) || shortens_path(np, x, y, 0, -1, s))
                    break ;

            x = holeX + shift;
            y = holeY - d + shift;

            if (x >= 0 && x < s && y >= 0 && y < s)
                if (shortens_path(np, x, y, -1, 0, s) || shortens_path(np, x, y, 0, 1, s))
                    break ;

            x = holeX - shift;
            y = holeY - d + shift;

            if (x >= 0 && x < s && y >= 0 && y < s)
                if (shortens_path(np, x, y, 1, 0, s) || shortens_path(np, x, y, 0, 1, s))
                    break ;
        }

        if (shift != d)
            break;

        ++dist;
    }

    return ret + dist * 2 * ONE_MOVE;
}

uint64_t node_value(uint64_t h, uint64_t g, uint64_t weight)
{
    return g * ONE_MOVE + h * weight / ONE_MOVE;
}
