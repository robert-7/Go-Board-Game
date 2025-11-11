#ifndef GAME_MOVE_H
#define GAME_MOVE_H

#include "game/Board.h"

// Simple value object representing a move request: a point and the stone
// color to place there.
struct Move {
    Point point{};
    Stone stone = Stone::Empty;
};

#endif // GAME_MOVE_H
