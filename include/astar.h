#ifndef ASTAR_H
# define ASTAR_H

# include "heuristic.h"
# include "npuzzle.h"

void launch_astar(NPuzzle *np, heuristic_t h, uint32_t weight);

#endif
