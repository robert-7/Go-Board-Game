#include "game/Board.h"

#include <cassert>
#include <utility>

auto Board::is_on_board(Point point) -> bool {
    const auto [ix, iy] = to_index(point);
    return ix >= 0 && ix < SIZE && iy >= 0 && iy < SIZE;
}

void Board::clear() {
    for (auto &column : stones_) {
        column.fill(Stone::Empty);
    }
    clear_liberties();
    clear_captured_groups();
}

void Board::clear_liberties() {
    for (auto &column : liberties_) {
        column.fill(-1);
    }
}

auto Board::stone_at(Point point) const -> Stone {
    assert(is_on_board(point));
    const auto [ix, iy] = to_index(point);
    return stones_[ix][iy];
}

void Board::place_stone(Point point, Stone stone) {
    assert(is_on_board(point));
    const auto [ix, iy] = to_index(point);
    stones_[ix][iy] = stone;
}

void Board::remove_stone(Point point) { place_stone(point, Stone::Empty); }

auto Board::is_empty(Point point) const -> bool {
    return stone_at(point) == Stone::Empty;
}

auto Board::liberty(Point point) const -> int {
    assert(is_on_board(point));
    const auto [ix, iy] = to_index(point);
    return liberties_[ix][iy];
}

auto Board::mutable_liberty(Point point) -> int & {
    assert(is_on_board(point));
    const auto [ix, iy] = to_index(point);
    return liberties_[ix][iy];
}

void Board::set_liberty(Point point, int value) { mutable_liberty(point) = value; }

auto Board::captured_groups() -> CaptureGroups & { return captured_groups_; }

auto Board::captured_groups() const -> const CaptureGroups & {
    return captured_groups_;
}

void Board::add_captured_group(CaptureGroup group) {
    captured_groups_.push_back(std::move(group));
}

void Board::clear_captured_groups() { captured_groups_.clear(); }

auto Board::to_index(Point point) -> std::pair<int, int> {
    return {point.x + CENTER, point.y + CENTER};
}

auto Board::index_to_point(int x, int y) -> Point { return {x - CENTER, y - CENTER}; }
