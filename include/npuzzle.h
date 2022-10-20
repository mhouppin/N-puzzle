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
    uint64_t h;
    uint64_t g;
    struct NPuzzle_ *parent;
}
NPuzzle;

int npuzzle_init(NPuzzle *np, const char *filename);
int npuzzle_init_rand(NPuzzle *np, size_t size);
void npuzzle_destroy(NPuzzle *np);
void untyped_npuzzle_destroy(void *np);
int npuzzle_solved(const NPuzzle *np);
int npuzzle_is_solvable(const NPuzzle *np);
void npuzzle_apply(NPuzzle *np, uint16_t move);
NPuzzle *npuzzle_dup(NPuzzle *np);
int npuzzle_comp_state(const void *l, const void *r);
int npuzzle_comp_stateptr(const void *l, const void *r);
int npuzzle_comp_value(const void *l, const void *r);

#endif
