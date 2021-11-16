#include "ft_string.h"
#include "zobrist.h"

uint64_t move_zobrist(uint16_t pieceIdx, uint16_t fromIdx, uint16_t toIdx)
{
    uint64_t h1 = pieceIdx;
    uint64_t h2 = fromIdx;
    uint64_t h3 = toIdx;
    uint64_t h4 = UINT64_C(0x0123456789ABCDEF);

    if (h2 > h3)
        ft_swap(&h2, &h3, 8);

    h1 *= h4;
    h2 *= h4;
    h3 *= h4;

    h1 = (h1 << 42) | (h1 >> 22);
    h2 = (h2 << 21) | (h2 >> 43);

    return h1 ^ h2 ^ h3;
}
