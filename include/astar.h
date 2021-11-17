#ifndef ASTAR_H
# define ASTAR_H

# include <stdbool.h>
# include "heuristic.h"
# include "npuzzle.h"

void launch_astar(NPuzzle *np, heuristic_t h, size_t maxNodes, bool verbose);

#endif
