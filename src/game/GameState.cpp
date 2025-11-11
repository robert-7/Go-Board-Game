#include "game/GameState.h"

void GameState::set_cursor(int x, int y) {
    cursor_x_ = x;
    cursor_y_ = y;
}

void GameState::move_cursor(int dx, int dy) {
    cursor_x_ += dx;
    cursor_y_ += dy;
}

void GameState::set_cursor_x(int x) { cursor_x_ = x; }

void GameState::set_cursor_y(int y) { cursor_y_ = y; }

auto GameState::cursor_x() const -> int { return cursor_x_; }

auto GameState::cursor_y() const -> int { return cursor_y_; }

void GameState::set_current_player(int color) { current_player_ = color; }

auto GameState::current_player() const -> int { return current_player_; }

void GameState::advance_turn() { current_player_ = current_player_ == 1 ? 2 : 1; }

void GameState::request_restart() { restart_pending_ = true; }

void GameState::cancel_restart() { restart_pending_ = false; }

auto GameState::restart_pending() const -> bool { return restart_pending_; }

void GameState::reset_animation() { animation_time_ = 0.0F; }

void GameState::advance_animation(float delta) { animation_time_ += delta; }

void GameState::set_animation_time(float time) { animation_time_ = time; }

auto GameState::animation_time() const -> float { return animation_time_; }
