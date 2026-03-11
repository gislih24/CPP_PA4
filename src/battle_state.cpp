#include "battle_state.hpp"
#include "enemy.hpp"
#include "game_state.hpp"
#include "player_character.hpp"
#include <iostream>
#include <print>
#include <string>

void BattleState::render() override {
    std::print("Woe! A fiend is upon ye!\n");

    while (in_battle && (pc.hp > 0) && (enemy.hp > 0)) {
        // player turn
        std::print("Choose an action:\n1. attack\n0. flee\n");
        std::getline(std::cin, line);
        int choice = std::stoi(line);
        switch (choice) {
        case 1:
            // attack
            damage_dealt = pc.attack(enemy);
            if (damage_dealt > 0) {
                std::print("You hit the enemy and dealt {} damage\n",
                           damage_dealt);
            }
            if (!enemy.is_alive()) {
                in_battle = false;
            }
            break;
        case 0:
            // flee
            std::print("You flee... a coward's choice\n");
            in_battle = false;
            break;
        default:
            std::print("Invalid choice\n");
            continue;
        }

        if (!in_battle || !enemy.is_alive()) {
            break;
        }

        // enemy's turn
        damage_dealt = enemy.attack(pc);
        std::print("You were hit for {} damage\n", damage_dealt);
        if (!pc.is_alive()) {
            in_battle = false;
            break;
        }
    }
    if (!pc.is_alive()) {
        std::print("You have died... shameful display!\n");
    } else if (!enemy.is_alive()) {
        std::print("You emerge victorious!\n");
    }
}

void BattleState::handle_input() override {}

void BattleState::update() override {}
