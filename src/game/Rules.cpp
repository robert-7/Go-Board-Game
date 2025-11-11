#include "game/Rules.h"

#include <array>
#include <iostream>
#include <utility>

#include "game/Board.h"
#include "game/GameSession.h"

namespace {

void collect_block(GameSession &session, Point point, Stone target_color,
                   Board::CaptureGroup &removed_group) {
    if (!Board::is_on_board(point)) {
        return;
    }

    if (session.board.stone_at(point) != target_color) {
        return;
    }

    session.board.remove_stone(point);
    std::cout << "Jump from " << point.x << point.y << '\n';

    removed_group.emplace_back(point, target_color);

    collect_block(session, Point{point.x - 1, point.y}, target_color, removed_group);
    collect_block(session, Point{point.x + 1, point.y}, target_color, removed_group);
    collect_block(session, Point{point.x, point.y - 1}, target_color, removed_group);
    collect_block(session, Point{point.x, point.y + 1}, target_color, removed_group);
}

void remove_block(GameSession &session, Point point, Stone target_color) {
    Board::CaptureGroup removed{};
    collect_block(session, point, target_color, removed);
    if (!removed.empty()) {
        session.board.add_captured_group(std::move(removed));
    }
}

auto check_liberties(GameSession &session, Point point, Point origin,
                     Stone target_color) -> int {
    if (!Board::is_on_board(point)) {
        return 0;
    }

    const auto stone = session.board.stone_at(point);
    if (is_empty(stone)) {
        return 1;
    }
    if (stone != target_color) {
        return 0;
    }

    auto &liberty_value = session.board.mutable_liberty(point);
    if (liberty_value != -1) {
        return liberty_value;
    }

    const int direcx = point.x - origin.x;
    const int direcy = point.y - origin.y;
    if ((direcx >= 0 &&
         check_liberties(session, Point{point.x + 1, point.y}, origin, target_color)) ||
        (direcx <= 0 &&
         check_liberties(session, Point{point.x - 1, point.y}, origin, target_color)) ||
        (direcy >= 0 &&
         check_liberties(session, Point{point.x, point.y + 1}, origin, target_color)) ||
        (direcy <= 0 &&
         check_liberties(session, Point{point.x, point.y - 1}, origin, target_color))) {
        liberty_value = 1;
    } else {
        liberty_value = 0;
    }
    return liberty_value;
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
    board.clear_liberties();

    const Stone opponent = stone_color == Stone::Black ? Stone::White : Stone::Black;
    int has_liberties = 0;

    const std::array<Point, 4> neighbors{{
        Point{move_point.x - 1, move_point.y},
        Point{move_point.x + 1, move_point.y},
        Point{move_point.x, move_point.y - 1},
        Point{move_point.x, move_point.y + 1},
    }};

    for (const auto &neighbor : neighbors) {
        if (!Board::is_on_board(neighbor)) {
            continue;
        }

        if (board.stone_at(neighbor) == stone_color) {
            continue;
        }

        if (stone_color == Stone::Empty) {
            has_liberties = 1;
        } else if (!check_liberties(session, neighbor, neighbor, opponent)) {
            has_liberties = 1;
            remove_block(session, neighbor, opponent);
        }
    }

    if (!has_liberties &&
        !check_liberties(session, move_point, move_point, stone_color)) {
        auto &captures = board.captured_groups();
        const auto previous_capture_count = captures.size();
        remove_block(session, move_point, stone_color);
        while (captures.size() > previous_capture_count) {
            captures.pop_back();
        }
        return 0;
    }

    return 1;
}

} // namespace rules
