#pragma once

#include <memory>

#include "world.hpp"

class GameState;

class Game {
  public:
    explicit Game(std::unique_ptr<GameState> initial_state);

    void run();

    void request_state_change(std::unique_ptr<GameState> next_state);
    void quit() noexcept;

    bool is_running() const noexcept {
        return game_is_running_;
    }

    World& get_world() noexcept {
        return world_;
    }

    const World& get_world() const noexcept {
        return world_;
    }

  private:
    void apply_pending_state_change();

    World world_{};
    std::unique_ptr<GameState> current_state_;
    std::unique_ptr<GameState> pending_state_;
    bool game_is_running_{true};
};