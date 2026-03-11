#pragma once

#include "enemy.hpp"
#include "game_state.hpp"
#include "player_character.hpp"
#include <iostream>
#include <print>
#include <string>

class BattleState final : public GameState {
  public:
    PlayerCharacter pc = PlayerCharacter{};
    Enemy enemy = Enemy{};
    std::string line;
    bool in_battle = true;
    int damage_dealt;

    BattleState();
    void render() override;
    void handle_input() override;
    void update() override;
};