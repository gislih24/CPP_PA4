#pragma once

#include "game_state.hpp"
#include <string>
#include <vector>

class MainMenuState final : public GameState {
  public:
    void on_enter(Game& game) override;
    void render(const Game& game) const override;
    void handle_input(Game& game, std::string_view input) override;

  private:
    void refresh_load_menu(const Game& game);
    void handle_main_menu_input(Game& game, std::string_view input);
    void handle_load_menu_input(Game& game, std::string_view input);

    std::string message_;
    std::vector<std::string> available_saves_;
    bool load_menu_active_{false};
};
