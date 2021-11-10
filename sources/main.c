#include <stdlib.h>
#include "astar.h"
#include "ft_stdio.h"
#include "ft_string.h"
#include "npuzzle.h"
#include "heuristic.h"

int main(int argc, char **argv)
{
    NPuzzle np;

    if (argc == 1)
    {
        ft_printf(" usage: %s puzzle_file [options]\n", *argv);
        return 1;
    }

    if (npuzzle_init(&np, argv[1]))
        return 1;

    heuristic_t h = &manhattan;
    uint32_t weight = ONE_MOVE;

    for (int i = 2; i < argc; ++i)
    {
        if (!ft_memcmp(argv[i], "--weight=", 9))
            weight = (uint32_t)(strtod(argv[i] + 9, NULL) * ONE_MOVE);
        else if (!ft_strcmp(argv[i], "--hole"))
            h = &hole_manhattan;
    }

    launch_astar(&np, h, weight);

    free(np.board);

    return 0;
}
