#pragma once

#include "game_state.hpp"
#include <vector>

class ExploreState final : public GameState {
  public:
    void on_enter(Game& game) override;
    void render(const Game& game) const override;
    void handle_input(Game& game, std::string_view input) override;

  private:
    std::vector<std::string> overworld_map_ = {};
    std::vector<std::string> status_display_ = {};
    std::vector<std::string> action_menu_ = {};

    enum class Action {
        BATTLE = 1,
        QUIT = 2,
        MOVE_LEFT = 3,
        MOVE_RIGHT = 4,
        MOVE_UP = 5,
        MOVE_DOWN = 6
    };

    void clear_message_vectors();
    void build_map(const Game& game) const;
    void move(Game& game, Action);

    std::vector<Action> available_actions_ = {
        Action::BATTLE,     Action::QUIT,    Action::MOVE_LEFT,
        Action::MOVE_RIGHT, Action::MOVE_UP, Action::MOVE_DOWN};
};