#pragma once

#include "enemy.hpp"
#include "game_state.hpp"
#include "player_character.hpp"
#include <iostream>
#include <print>
#include <string>
#include <vector>

class BattleState final : public GameState {
  public:
    explicit BattleState(Enemy* enemy);
    void on_enter(Game& game) override;
    void render(const Game& game) const override;
    void handle_input(Game& game, std::string_view input) override;

  private:
    enum class Outcome { Ongoing, Victory, Fled, Defeat };

    // Actions that occurred in combat,
    // e.g.: "player attacks for 7 damage", "enemy attacks for 4 damage",
    // "player has lost status effect: 2x damage"
    std::vector<std::string> combat_log_ = {};
    // The current status of things,
    // e.g.: "enemy health: 7/15 HP" "player health: 9/12 HP".
    std::vector<std::string> status_display_ = {};
    // Actions currently available to the player,
    // e.g.: "Choose an action: 1. Attack, 2. Inventory, 3. Flee"
    std::vector<std::string> action_menu_ = {};

    Enemy* enemy_{nullptr};
    Position encounter_position_{};
    std::string enemy_name_{};
    Outcome outcome_{Outcome::Ongoing};

    void clear_message_vectors();
    void rebuild_status(const Game& game);
};
