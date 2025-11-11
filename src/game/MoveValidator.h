#ifndef GAME_MOVE_VALIDATOR_H
#define GAME_MOVE_VALIDATOR_H

class GameSession;
struct Move;

// MoveValidator performs legality checks before a move mutates the real
// GameSession state. It intentionally avoids mutating the provided session.
// Current checks: bounds, occupancy, and self-capture. Hooks are prepared for
// future ko/history rules.
class MoveValidator {
  public:
    // Returns true when the move is legal for the session's current board.
    static auto is_legal(const GameSession &session, const Move &move) -> bool;
};

#endif // GAME_MOVE_VALIDATOR_H
