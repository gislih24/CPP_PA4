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

    BattleState() {
        pc.stats.attack = 2;
        pc.stats.defence = 1;
        pc.stats.max_hp = 10;
        pc.hp = pc.stats.max_hp;

        enemy.stats.attack = 4;
        enemy.stats.defence = 1;
        enemy.stats.max_hp = 10;
        enemy.hp = enemy.stats.max_hp;
    }

    void render() override;

    void handle_input() override;

    void update() override;
};