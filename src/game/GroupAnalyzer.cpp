#include "game/GroupAnalyzer.h"

#include "game/Board.h"

#include <array>
#include <optional>
#include <stack>
#include <utility>

namespace {

constexpr auto GRID_SIZE = Board::SIZE;

using VisitedGrid = std::array<std::array<bool, GRID_SIZE>, GRID_SIZE>;

void clear_grid(VisitedGrid &grid) {
    for (auto &column : grid) {
        column.fill(false);
    }
}

} // namespace

GroupAnalyzer::GroupAnalyzer(const Board &board) : board_(board) {}

auto GroupAnalyzer::analyze(Point start) const -> std::optional<GroupAnalysis> {
    if (!Board::is_on_board(start)) {
        return std::nullopt;
    }

    const Stone color = board_.stone_at(start);
    if (color == Stone::Empty) {
        return std::nullopt;
    }

    VisitedGrid visited{};
    clear_grid(visited);

    VisitedGrid liberty_seen{};
    clear_grid(liberty_seen);

    GroupAnalysis analysis;
    analysis.color = color;

    std::stack<Point> stack;
    stack.push(start);

    while (!stack.empty()) {
        const Point current = stack.top();
        stack.pop();

        const auto [ix, iy] = to_index(current);
        if (visited[ix][iy]) {
            continue;
        }
        visited[ix][iy] = true;

        analysis.stones.push_back(current);

        for (const auto &neighbor : neighbors(current)) {
            if (!Board::is_on_board(neighbor)) {
                continue;
            }

            const auto neighbor_color = board_.stone_at(neighbor);
            if (neighbor_color == color) {
                const auto [nx, ny] = to_index(neighbor);
                if (!visited[nx][ny]) {
                    stack.push(neighbor);
                }
                continue;
            }

            if (neighbor_color == Stone::Empty) {
                const auto [nx, ny] = to_index(neighbor);
                if (!liberty_seen[nx][ny]) {
                    liberty_seen[nx][ny] = true;
                    analysis.liberties.push_back(neighbor);
                }
            }
        }
    }

    return analysis;
}

auto GroupAnalyzer::to_index(Point point) -> std::pair<int, int> {
    return {point.x + Board::CENTER, point.y + Board::CENTER};
}

auto GroupAnalyzer::neighbors(Point point) -> std::array<Point, 4> {
    return {Point{point.x - 1, point.y}, Point{point.x + 1, point.y},
            Point{point.x, point.y - 1}, Point{point.x, point.y + 1}};
}
