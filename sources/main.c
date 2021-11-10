#include <stdlib.h>
#include "ft_stdio.h"
#include "npuzzle.h"

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

    for (size_t p = 0; p < np.size * np.size; ++p)
        ft_printf("%3hu%c", np.board[p], (p % np.size) != np.size - 1 ? ' ' : '\n');

    free(np.board);

    return 0;
}
