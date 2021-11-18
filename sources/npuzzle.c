#include <stdio.h>
#include <stdlib.h>
#include "ft_stdio.h"
#include "ft_string.h"
#include "heuristic.h"
#include "npuzzle.h"
#include "zobrist.h"

int npuzzle_init(NPuzzle *np, const char *filename)
{
    FILE *f = fopen(filename, "r");

    if (f == NULL)
    {
        perror("Parsing error: unable to open n-puzzle file");
        return -1;
    }

    ssize_t r;
    char *line = NULL;
    size_t lineSize = 0;

    *np = (NPuzzle) {
        .size = 0,
        .board = NULL,
        .zobrist = 0,
        .holeIdx = 0,
        .h = 0,
        .g = 0,
        .parent = NULL
    };

    while ((r = getline(&line, &lineSize, f)) > 0)
    {
        if (ft_strlen(line) != (size_t)r)
        {
            ft_dputstr("Parsing error: nullbytes in string\n", STDERR_FILENO);
            goto npuzzle_init_error;
        }

        line[r - 1] = '\0';
        char *ptr = line + ft_strspn(line, " \t");

        // Empty line or start of comment, skip line.
        if (*ptr == '#' || *ptr == '\0')
            continue ;

        np->size = strtoul(ptr, &ptr, 10);

        ptr += ft_strspn(line, " \t");

        // If there's more info on the line, that's an error.
        if (*ptr != '#' && *ptr != '\0')
        {
            ft_dprintf(STDERR_FILENO, "Parsing error: invalid data '%s' after puzzle size\n", ptr);
            goto npuzzle_init_error;
        }

        break ;
    }

    if (np->size == 0 || np->size >= 256)
    {
        ft_dputstr("Parsing error: missing or invalid puzzle size\n", STDERR_FILENO);
        goto npuzzle_init_error;
    }

    np->board = malloc(2 * np->size * np->size);

    if (np->board == NULL)
    {
        perror("Parsing error");
        goto npuzzle_init_error;
    }

    size_t yLen = 0;

    while ((r = getline(&line, &lineSize, f)) > 0)
    {
        if (ft_strlen(line) != (size_t)r)
        {
            ft_dputstr("Parsing error: nullbytes in string\n", STDERR_FILENO);
            goto npuzzle_init_error;
        }

        line[r - 1] = '\0';
        char *ptr = line + ft_strspn(line, " \t");

        // Empty line or start of comment, skip line.
        if (*ptr == '#' || *ptr == '\0')
            continue ;

        if (yLen == np->size)
        {
            ft_dprintf(STDERR_FILENO, "Parsing error: '%s' found even though the puzzle is complete\n", ptr);
            goto npuzzle_init_error;
        }

        size_t xLen;

        for (xLen = 0; xLen < np->size; ++xLen)
        {
            uint16_t value = strtoul(ptr, &ptr, 10);

            if (value > np->size * np->size)
            {
                if (*ptr == '\0')
                    ft_dputstr("Parsing error: missing pieces in line\n", STDERR_FILENO);
                else
                    ft_dprintf(STDERR_FILENO, "Parsing error: invalid piece index '%u'\n", (unsigned int)value);
                goto npuzzle_init_error;
            }

            np->board[yLen * np->size + xLen] = value;
            ptr += ft_strspn(ptr, " \t");
        }

        if (*ptr != '#' && *ptr != '\0')
        {
            ft_dprintf(STDERR_FILENO, "Parsing error: extra data '%s' after piece indexes\n", ptr);
            goto npuzzle_init_error;
        }

        ++yLen;
    }

    for (size_t sq1 = 0; sq1 < np->size * np->size; ++sq1)
        for (size_t sq2 = sq1 + 1; sq2 < np->size * np->size; ++sq2)
            if (np->board[sq1] == np->board[sq2])
            {
                ft_dputstr("NPuzzle Error: duplicate piece\n", STDERR_FILENO);
                goto npuzzle_init_error;
            }

    // Now that we know the board is valid, initialize other fields.
    size_t holeIdx;
    for (holeIdx = 0; np->board[holeIdx] != 0; ++holeIdx);
    np->holeIdx = holeIdx;
    np->zobrist = 0;

    // Edit array values to tag corresponding squares.
    uint16_t *tagArray = malloc(np->size * np->size * sizeof(uint16_t));

    if (tagArray == NULL)
    {
        perror("NPuzzle error");
        goto npuzzle_init_error;
    }

    size_t counter = 0;

    // Now initialize the tag array values in a spiral shape.
    for (size_t layer = 0; layer < np->size / 2; ++layer)
    {
        size_t boxMin = layer;
        size_t boxMax = np->size - layer - 1;

        for (size_t x = boxMin; x <= boxMax; ++x)
            tagArray[boxMin * np->size + x] = ++counter;

        for (size_t y = boxMin + 1; y <= boxMax; ++y)
            tagArray[y * np->size + boxMax] = ++counter;

        for (size_t x = boxMax - 1; x + 1 > boxMin; --x)
            tagArray[boxMax * np->size + x] = ++counter;

        for (size_t y = boxMax - 1; y > boxMin; --y)
            tagArray[y * np->size + boxMin] = ++counter;
    }

    // Add the hole (represented by a 0) to the tag array.
    tagArray[(np->size / 2) * np->size + (np->size - 1) / 2] = 0;

    // Now replace the piece values in the board by their corresponding square.
    for (size_t sq = 0; sq < np->size * np->size; ++sq)
        for (size_t asq = 0; asq < np->size * np->size; ++asq)
            if (np->board[sq] == tagArray[asq])
            {
                np->board[sq] = asq;
                break ;
            }

    free(tagArray);
    fclose(f);
    free(line);
    return 0;

npuzzle_init_error:
    free(np->board);
    fclose(f);
    free(line);
    return -1;
}

void npuzzle_destroy(NPuzzle *np)
{
    free(np->board);
    free(np);
}

void untyped_npuzzle_destroy(void *np)
{
    npuzzle_destroy(*(NPuzzle **)np);
}

int npuzzle_solved(const NPuzzle *np)
{
    for (size_t sq = 0; sq < np->size * np->size; ++sq)
        if (np->board[sq] != sq)
            return 0;

    return 1;
}

void npuzzle_apply(NPuzzle *np, uint16_t squareIdx)
{
    uint16_t piece = np->board[squareIdx];
    uint16_t holeValue = np->board[np->holeIdx];

    np->zobrist ^= move_zobrist(piece, squareIdx, np->holeIdx);
    np->board[np->holeIdx] = piece;
    np->board[squareIdx] = holeValue;
    np->holeIdx = squareIdx;
    np->g++;
}

NPuzzle *npuzzle_dup(NPuzzle *np)
{
    NPuzzle *new = malloc(sizeof(NPuzzle));

    if (new == NULL)
    {
        perror("NPuzzle error");
        return NULL;
    }

    new->size = np->size;
    new->board = malloc(2 * np->size * np->size);

    if (new->board == NULL)
    {
        perror("NPuzzle error");
        free(new);
        return NULL;
    }

    ft_memcpy(new->board, np->board, 2 * np->size * np->size);
    new->holeIdx = np->holeIdx;
    new->zobrist = np->zobrist;
    new->h = np->h;
    new->g = np->g;
    new->parent = np;
    return new;
}

int npuzzle_comp_state(const void *l, const void *r)
{
    const NPuzzle *left = l;
    const NPuzzle *right = r;

    if (left->zobrist < right->zobrist)
        return -1;
    else if (left->zobrist > right->zobrist)
        return 1;
    else
        return ft_memcmp(left->board, right->board, 2 * left->size * left->size);
}

int npuzzle_comp_stateptr(const void *l, const void *r)
{
    return npuzzle_comp_state(*(NPuzzle **)l, *(NPuzzle **)r);
}

int npuzzle_comp_value(const void *l, const void *r)
{
    extern uint64_t Weight;
    const NPuzzle *left = *(NPuzzle **)l;
    const NPuzzle *right = *(NPuzzle **)r;
    const uint64_t lvalue = node_value(left->h, left->g, Weight);
    const uint64_t rvalue = node_value(right->h, right->g, Weight);

    if (lvalue < rvalue)
        return -1;
    else if (lvalue > rvalue)
        return 1;
    else
        return (int)left->g - (int)right->g;
}
