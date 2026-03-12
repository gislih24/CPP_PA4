#pragma once

#include "game_state.hpp"

class ExploreState final : public GameState {
  public:
    void on_enter(Game& game) override;
    void render(const Game& game) const override;
    void handle_input(Game& game, std::string_view input) override;

  private:
    std::string message_;
};