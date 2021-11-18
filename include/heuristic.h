#ifndef HEURISTIC_H
# define HEURISTIC_H

# include "npuzzle.h"

enum { ONE_MOVE = 1024 };

typedef uint64_t (*heuristic_t)(const NPuzzle *);

uint64_t manhattan(const NPuzzle *np);
uint64_t hole_manhattan(const NPuzzle *np);
uint64_t bwp_manhattan(const NPuzzle *np);
uint64_t node_value(uint64_t h, uint64_t g, uint64_t weight);

#endif
