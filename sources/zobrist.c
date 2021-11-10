#include "ft_string.h"
#include "zobrist.h"

uint64_t move_zobrist(uint16_t pieceIdx, uint16_t fromIdx, uint16_t toIdx)
{
    uint64_t h1 = pieceIdx;
    uint64_t h2 = fromIdx;
    uint64_t h3 = toIdx;

    if (h2 > h3)
        ft_swap(&h2, &h3, 8);

    // First apply Cantor Pairing function.
    uint64_t h = (h1 + h2) * (h1 + h2 + 1) / 2 + h2;
    h = (h + h3) * (h1 + h2 + 1) / 2 + h3;

    // Then scramble bits with a large multiplication.
    return h * UINT64_C(0x0123456789ABCDEF);
}
