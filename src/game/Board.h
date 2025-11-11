#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include <array>
#include <list>
#include <vector>

class Board {
  public:
    static constexpr int SIZE = 19;
    static constexpr int CENTER = SIZE / 2;

    using Grid = std::array<std::array<int, SIZE>, SIZE>;
    using CaptureGroup = std::vector<int>;
    using CaptureGroups = std::list<CaptureGroup>;

    void clear();
    void clear_liberties();

    auto stones() -> Grid &;
    auto stones() const -> const Grid &;

    auto liberties() -> Grid &;
    auto liberties() const -> const Grid &;

    auto captured_groups() -> CaptureGroups &;
    auto captured_groups() const -> const CaptureGroups &;

    void add_captured_group(CaptureGroup group);
    void clear_captured_groups();

  private:
    Grid stones_{};
    Grid liberties_{};
    CaptureGroups captured_groups_;
};

#endif // GAME_BOARD_H
