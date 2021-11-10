#ifndef NPUZZLE_H
# define NPUZZLE_H

# include <stddef.h>
# include <stdint.h>

// Structure for holding a single n-puzzle.
typedef struct NPuzzle_
{
    size_t size;
    uint16_t *board;
    uint64_t zobrist;
    uint16_t holeIdx;
}
NPuzzle;

int npuzzle_init(NPuzzle *np, const char *filename);
void npuzzle_apply(NPuzzle *np, uint16_t move);

#endif
