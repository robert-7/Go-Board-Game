#include "game/MoveValidator.h"

#include "game/Board.h"
#include "game/GameSession.h"
#include "game/GroupAnalyzer.h"
#include "game/Move.h"

#include <array>
#include <optional>

// Implementation notes:
// - This validator copies the board and simulates the placement to determine
//   whether the move would be self-capture. It does not mutate the real
//   session. Ko/history detection is left as a placeholder hook.

auto MoveValidator::is_legal(const GameSession &session, const Move &move) -> bool {
    const auto &board = session.board;

    // Bounds check
    if (!Board::is_on_board(move.point)) {
        return false;
    }

    // Occupancy check
    if (!board.is_empty(move.point)) {
        return false;
    }

    // Copy the board to simulate the move without mutating session.
    Board temp = board; // default copy is fine for arrays and lists

    // Place the stone on the temp board
    temp.place_stone(move.point, move.stone);

    const Stone opponent = move.stone == Stone::Black ? Stone::White : Stone::Black;

    // Check captures of adjacent opponent groups. If any adjacent opponent group
    // loses all liberties after the move, those stones would be captured and
    // therefore the move cannot be a self-capture.
    GroupAnalyzer analyzer(temp);

    bool any_capture = false;
    const auto neighbors = std::array<Point, 4>{
        Point{move.point.x - 1, move.point.y}, Point{move.point.x + 1, move.point.y},
        Point{move.point.x, move.point.y - 1}, Point{move.point.x, move.point.y + 1}};

    for (const auto &neighbor : neighbors) {
        if (!Board::is_on_board(neighbor)) {
            continue;
        }

        if (temp.stone_at(neighbor) != opponent) {
            continue;
        }

        const auto analysis = analyzer.analyze(neighbor);
        if (!analysis.has_value()) {
            continue;
        }

        if (!analysis->has_liberties()) {
            any_capture = true;
            break;
        }
    }

    if (any_capture) {
        // The move captures opponent stones; therefore it is legal even if the
        // placed stone itself might appear to have no liberties.
        return true;
    }

    // No captures; check whether the newly-placed stone's group has liberties.
    GroupAnalyzer self_analyzer(temp);
    const auto own_group = self_analyzer.analyze(move.point);

    const bool move_has_liberty = own_group.has_value() && own_group->has_liberties();

    if (!move_has_liberty) {
        // Self-capture with no captures is illegal.
        return false;
    }

    // Placeholder for ko/history rules: we could compute the board hash after
    // the move and consult session history to disallow repeating positions.
    // For now, return true as the basic rules pass.

    return true;
}
