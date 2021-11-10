#include <stdio.h>
#include <stdlib.h>
#include "ft_stdio.h"
#include "ft_string.h"
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
    size_t size = 0;
    np->size = 0;

    while ((r = getline(&line, &size, f)) > 0)
    {
        if (ft_strlen(line) != (size_t)r)
        {
            ft_dputstr("Parsing error: nullbytes in string\n", STDERR_FILENO);
            return -1;
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
            return -1;
        }

        break ;
    }

    if (np->size == 0 || np->size >= 256)
    {
        ft_dputstr("Parsing error: missing or invalid puzzle size\n", STDERR_FILENO);
        return -1;
    }

    np->board = malloc(2 * np->size * np->size);

    if (np->board == NULL)
    {
        perror("Parsing error");
        return -1;
    }

    size_t yLen = 0;

    while ((r = getline(&line, &size, f)) > 0)
    {
        if (ft_strlen(line) != (size_t)r)
        {
            ft_dputstr("Parsing error: nullbytes in string\n", STDERR_FILENO);
            free(np->board);
            return -1;
        }

        line[r - 1] = '\0';
        char *ptr = line + ft_strspn(line, " \t");

        // Empty line or start of comment, skip line.
        if (*ptr == '#' || *ptr == '\0')
            continue ;

        if (yLen == np->size)
        {
            ft_dprintf(STDERR_FILENO, "Parsing error: '%s' found even though the puzzle is complete\n", ptr);
            free(np->board);
            return -1;
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
                free(np->board);
                return -1;
            }

            np->board[yLen * np->size + xLen] = value;
            ptr += ft_strspn(ptr, " \t");
        }

        if (*ptr != '#' && *ptr != '\0')
        {
            ft_dprintf(STDERR_FILENO, "Parsing error: extra data '%s' after piece indexes\n", ptr);
            free(np->board);
            return -1;
        }

        ++yLen;
    }

    for (size_t sq1 = 0; sq1 < np->size * np->size; ++sq1)
        for (size_t sq2 = sq1 + 1; sq2 < np->size * np->size; ++sq2)
            if (np->board[sq1] == np->board[sq2])
            {
                ft_dputstr("NPuzzle Error: duplicate piece\n", STDERR_FILENO);
                free(np->board);
                return -1;
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
        free(np->board);
        return -1;
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

    return 0;
}

void npuzzle_apply(NPuzzle *np, uint16_t squareIdx)
{
    uint16_t piece = np->board[squareIdx];

    np->zobrist ^= move_zobrist(piece, squareIdx, np->holeIdx);
    np->board[np->holeIdx] = piece;
    np->board[squareIdx] = np->size * np->size;
    np->holeIdx = squareIdx;
}
