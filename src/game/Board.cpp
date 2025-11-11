#include "game/Board.h"

#include <utility>

void Board::clear() {
    for (auto &column : stones_) {
        column.fill(0);
    }
    clear_liberties();
    clear_captured_groups();
}

void Board::clear_liberties() {
    for (auto &column : liberties_) {
        column.fill(-1);
    }
}

auto Board::stones() -> Grid & { return stones_; }

auto Board::stones() const -> const Grid & { return stones_; }

auto Board::liberties() -> Grid & { return liberties_; }

auto Board::liberties() const -> const Grid & { return liberties_; }

auto Board::captured_groups() -> CaptureGroups & { return captured_groups_; }

auto Board::captured_groups() const -> const CaptureGroups & {
    return captured_groups_;
}

void Board::add_captured_group(CaptureGroup group) {
    captured_groups_.push_back(std::move(group));
}

void Board::clear_captured_groups() { captured_groups_.clear(); }
