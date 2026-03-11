#include "battle_state.hpp"
#include "enemy.hpp"
#include "game_state.hpp"
#include "player_character.hpp"
#include <iostream>
// #include <print>
#include <string>
#include <vector>

BattleState::BattleState() {
    pc.stats.attack = 2;
    pc.stats.defence = 1;
    pc.stats.max_hp = 10;
    pc.hp = pc.stats.max_hp;

    enemy.stats.attack = 4;
    enemy.stats.defence = 1;
    enemy.stats.max_hp = 10;
    enemy.hp = enemy.stats.max_hp;
}

void BattleState::on_enter(Game& game) {
    combat_log_.push_back("Woe, a fiend is upon ye!");
}

void BattleState::render(const Game& game) const {
    // For each of the message vectors
    for (const auto* message_vector :
         {&combat_log_, &status_display_, &action_menu_}) {
        // For each message in the current message vector
        for (const auto& message : *message_vector) {
            std::cout << message << '\n'; // Print it
        }
    }
}

void BattleState::handle_input(Game& game, std::string_view input) {
    clear_message_vectors();
    if (in_battle && (pc.hp > 0) && (enemy.hp > 0)) {
        // player turn
        action_menu_.push_back("Choose an action:\n1. attack\n0. flee\n");
        if (input == "1") {
            // attack
            damage_dealt = pc.attack(enemy);
            if (damage_dealt > 0) {

                action_menu_.push_back(
                    "Choose an action:\n1. attack\n0. flee\n");
                std::cout << "You hit the enemy and dealt {} damage\n"
                          << damage_dealt;
            }
            if (!enemy.is_alive()) {
                in_battle = false;
            }
        } else if (input == "2") {
            // flee
            std::cout << "You flee... a coward's choice\n";
            in_battle = false;
        }
    default:
            std::cout <<"Invalid choice\n");
            continue;
    }

    if (!in_battle || !enemy.is_alive()) {
        break;
    }

    // enemy's turn
    damage_dealt = enemy.attack(pc);
        std::cout <<"You were hit for {} damage\n", damage_dealt);
        if (!pc.is_alive()) {
            in_battle = false;
            break;
        }
}
if (!pc.is_alive()) {
        std::cout <<"You have died... shameful display!\n");
} else if (!enemy.is_alive()) {
        std::cout <<"You emerge victorious!\n");
}
}

void BattleState::clear_message_vectors() {
    combat_log_.clear();
    status_display_.clear();
    action_menu_.clear();
}