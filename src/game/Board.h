#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include <array>
#include <cassert>
#include <cstdint>
#include <list>
#include <utility>
#include <vector>

enum class Stone : std::uint8_t {
    Empty = 0,
    White = 1,
    Black = 2,
};

struct Point {
    int x = 0;
    int y = 0;

    constexpr Point() = default;
    constexpr Point(int x_coord, int y_coord) : x(x_coord), y(y_coord) {}
};

struct CapturedStone {
    Point location{};
    Stone color = Stone::Empty;

    constexpr CapturedStone() = default;
    constexpr CapturedStone(Point point, Stone stone_color)
        : location(point), color(stone_color) {}
};

inline auto is_empty(Stone stone) -> bool { return stone == Stone::Empty; }

class Board {
  public:
    static constexpr int SIZE = 19;
    static constexpr int CENTER = SIZE / 2;

    using Grid = std::array<std::array<Stone, SIZE>, SIZE>;
    using CaptureGroup = std::vector<CapturedStone>;
    using CaptureGroups = std::list<CaptureGroup>;

    static auto is_on_board(Point point) -> bool;

    void clear();

    auto stone_at(Point point) const -> Stone;
    void place_stone(Point point, Stone stone);
    void remove_stone(Point point);
    auto is_empty(Point point) const -> bool;

    auto captured_groups() -> CaptureGroups &;
    auto captured_groups() const -> const CaptureGroups &;

    void add_captured_group(CaptureGroup group);
    void clear_captured_groups();

    template <typename Func> void for_each_stone(Func &&fn) const {
        for (int column = 0; column < SIZE; ++column) {
            for (int row = 0; row < SIZE; ++row) {
                fn(index_to_point(column, row), stones_[column][row]);
            }
        }
    }

  private:
    static auto to_index(Point point) -> std::pair<int, int>;
    static auto index_to_point(int x, int y) -> Point;

    Grid stones_{};
    CaptureGroups captured_groups_;
};

#endif // GAME_BOARD_H
