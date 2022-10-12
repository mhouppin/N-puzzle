#include <stdlib.h>
#include "astar.h"
#include "ft_stdio.h"
#include "ft_string.h"
#include "hashtable.h"
#include "npuzzle.h"
#include "heuristic.h"

uint64_t Weight = ONE_MOVE;

void show_usage(const char *progName)
{
    ft_printf("Usage: %s <puzzle_file | map_size> [options]\n\n", progName);
    ft_printf("Available options:\n");
    ft_printf(" -h (or --help)    - Show this help.\n");
    ft_printf(" -v (or --verbose) - Show verbose output.\n");
    ft_printf(" --hole            - Uses a modified MH heuristic that takes into account the position of the hole.\n");
    ft_printf(" --bwp             - Uses a modified MH heuristic that penalizes forced backward moves.\n");
    ft_printf(" --manhattan       - Uses the Manhattan distance heuristic. (Used by default)\n");
    ft_printf(" --weight=W        - Set the A* static weighting of the heuristic to W. (default = 1.0)\n");
    ft_printf(" --max-nodes=M     - Forces the A* search to stop after M nodes allocated.\n");
    ft_printf(" --max-memory=M    - Sets the maximal memory usage, in MB, for running the A* search. (Default: 16 MB)\n");
    ft_printf("                     (Note that both --max-nodes and --max-memory override each other with priority to the last");
    ft_printf(" one in the arguments, if both are set.)\n");
}

int main(int argc, char **argv)
{
    NPuzzle np;
    bool verbose = false;

    if (argc == 1 || (argc == 2 && (ft_strcmp(argv[1], "-h") == 0 || ft_strcmp(argv[1], "--help") == 0)))
    {
        show_usage(argv[0]);
        return argc == 1;
    }

    {
        char *endPtr;
        size_t npuzzleSize = strtoul(argv[1], &endPtr, 10);

        if (*endPtr != '\0')
        {
            if (npuzzle_init(&np, argv[1]))
                return 1;
        }
        else if (npuzzleSize == 0 || npuzzleSize >= 256)
        {
            ft_printf("Invalid map size '%s'\n", argv[1]);
            return 1;
        }
        else if (npuzzle_init_rand(&np, npuzzleSize))
            return 1;
    }

    size_t singleNodeMemory = sizeof(NPuzzle) + np.size * np.size * 2 + sizeof(BucketEntry) + sizeof(BucketEntry *);
    // Round singleNodeMemory to the upper 32-byte boundary
    singleNodeMemory = (singleNodeMemory + 31) & ~0x1F;
    size_t maxNodes = 16777216ul / singleNodeMemory;

    heuristic_t h = &manhattan;

    for (int i = 2; i < argc; ++i)
    {
        if (!ft_memcmp(argv[i], "--weight=", 9))
            Weight = (uint64_t)(strtod(argv[i] + 9, NULL) * ONE_MOVE + 0.5);

        else if (!ft_strcmp(argv[i], "--hole"))
            h = &hole_manhattan;

        else if (!ft_memcmp(argv[i], "--max-nodes=", 12))
            maxNodes = (size_t)strtol(argv[i] + 12, NULL, 10);

        else if (!ft_memcmp(argv[i], "--max-memory=", 13))
            maxNodes = (size_t)strtol(argv[i] + 13, NULL, 10) * 1048576ul / singleNodeMemory;

        else if (!ft_strcmp(argv[i], "--manhattan"))
            h = &manhattan;

        else if (!ft_strcmp(argv[i], "--bwp"))
            h = &bwp_manhattan;

        else if (!ft_strcmp(argv[i], "-v") || !ft_strcmp(argv[i], "--verbose"))
            verbose = true;

        else
        {
            ft_printf("Unknown option: %s\n", argv[i]);
            return 1;
        }
    }

    launch_astar(&np, h, maxNodes, verbose);

    free(np.board);

    return 0;
}
