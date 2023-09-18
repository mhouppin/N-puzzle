#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <stdint.h>

uint64_t move_zobrist(uint16_t pieceIdx, uint16_t fromIdx, uint16_t toIdx);

#endif
