#ifndef GAME_STATE_H
#define GAME_STATE_H

class GameState {
  public:
    void set_cursor(int x, int y);
    void move_cursor(int dx, int dy);
    void set_cursor_x(int x);
    void set_cursor_y(int y);
    auto cursor_x() const -> int;
    auto cursor_y() const -> int;

    void set_current_player(int color);
    auto current_player() const -> int;
    void advance_turn();

    void request_restart();
    void cancel_restart();
    auto restart_pending() const -> bool;

    void reset_animation();
    void advance_animation(float delta);
    void set_animation_time(float time);
    auto animation_time() const -> float;

  private:
    int cursor_x_ = 0;
    int cursor_y_ = 0;
    int current_player_ = 1;
    bool restart_pending_ = false;
    float animation_time_ = 0.0F;
};

#endif // GAME_STATE_H
