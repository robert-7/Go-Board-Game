#include "game/Rules.h"

#include "game/Board.h"
#include "game/GameSession.h"
#include "game/GroupAnalyzer.h"

#include <array>
#include <optional>
#include <utility>
#include <vector>

namespace {

auto neighbors(Point point) -> std::array<Point, 4> {
    return {Point{point.x - 1, point.y}, Point{point.x + 1, point.y},
            Point{point.x, point.y - 1}, Point{point.x, point.y + 1}};
}

void restore_captures(Board &board, const std::vector<Board::CaptureGroup> &captures) {
    for (const auto &group : captures) {
        for (const auto &captured : group) {
            board.place_stone(captured.location, captured.color);
        }
    }
}

} // namespace

namespace rules {

void init_board(GameSession &session) { session.board.clear(); }

auto make_move(GameSession &session, Point move_point, Stone stone_color) -> int {
    auto &board = session.board;

    if (!Board::is_on_board(move_point)) {
        return 0;
    }

    if (!board.is_empty(move_point)) {
        return 0;
    }

    board.place_stone(move_point, stone_color);

    const Stone opponent = stone_color == Stone::Black ? Stone::White : Stone::Black;

    std::vector<Board::CaptureGroup> captured_groups;
    GroupAnalyzer analyzer(board);

    for (const auto &neighbor : neighbors(move_point)) {
        if (!Board::is_on_board(neighbor)) {
            continue;
        }

        if (board.stone_at(neighbor) != opponent) {
            continue;
        }

        const auto analysis = analyzer.analyze(neighbor);
        if (!analysis.has_value() || analysis->has_liberties()) {
            continue;
        }

        Board::CaptureGroup removed;
        removed.reserve(analysis->stones.size());
        for (const auto &stone_point : analysis->stones) {
            board.remove_stone(stone_point);
            removed.emplace_back(stone_point, opponent);
        }
        captured_groups.push_back(std::move(removed));
    }

    GroupAnalyzer self_analyzer(board);
    const auto own_group = self_analyzer.analyze(move_point);

    const bool move_has_liberty = own_group.has_value() && own_group->has_liberties();

    if (!move_has_liberty && captured_groups.empty()) {
        board.remove_stone(move_point);
        restore_captures(board, captured_groups);
        return 0;
    }

    for (auto &group : captured_groups) {
        board.add_captured_group(std::move(group));
    }

    return 1;
}

} // namespace rules
