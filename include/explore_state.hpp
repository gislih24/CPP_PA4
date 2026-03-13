#pragma once

#include "game_state.hpp"
#include <vector>

class ExploreState final : public GameState {
  public:
    explicit ExploreState(std::string status_message = {});

    void on_enter(Game& game) override;
    void render(const Game& game) const override;
    void handle_input(Game& game, std::string_view input) override;

  private:
    std::vector<std::string> overworld_map_ = {};
    std::vector<std::string> status_display_ = {};
    std::vector<std::string> action_menu_ = {};

    enum class Action { MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN };

    void clear_message_vectors();
    void build_map(const Game& game);
    void rebuild_ui(const Game& game);
    void save(Game& game, std::string_view input);
    void move(Game& game, Action action);

    std::string status_message_;
};
