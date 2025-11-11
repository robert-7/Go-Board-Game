#ifndef GAME_GROUP_ANALYZER_H
#define GAME_GROUP_ANALYZER_H

#include "game/Board.h"

#include <array>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

// Liberties are the empty intersections immediately adjacent to any stone in a
// connected group. A group with no liberties is captured.
struct GroupAnalysis {
    Stone color = Stone::Empty;
    std::vector<Point> stones;
    std::vector<Point> liberties;

    [[nodiscard]] auto has_liberties() const -> bool { return !liberties.empty(); }
};

class GroupAnalyzer {
  public:
    explicit GroupAnalyzer(const Board &board);

    // Returns the stones and liberties for the connected group rooted at
    // `start`. `std::nullopt` signals either an empty intersection or an
    // out-of-bounds request.
    [[nodiscard]] auto analyze(Point start) const -> std::optional<GroupAnalysis>;

  private:
    const Board &board_;

    [[nodiscard]] static auto to_index(Point point) -> std::pair<int, int>;
    [[nodiscard]] static auto neighbors(Point point) -> std::array<Point, 4>;
};

#endif // GAME_GROUP_ANALYZER_H
