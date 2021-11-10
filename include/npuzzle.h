#ifndef NPUZZLE_H
# define NPUZZLE_H

# include <stddef.h>
# include <stdint.h>

// Structure for holding a single n-puzzle.
typedef struct NPuzzle_
{
    size_t size;
    uint16_t *board;
}
NPuzzle;

#endif
