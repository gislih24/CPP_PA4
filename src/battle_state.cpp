#include "./include/battle_state.hpp"
#include "./include/enemy.hpp"
#include "./include/game_state.hpp"
#include "./include/player_character.hpp"
#include <format>
#include <iostream>
#include <print>
#include <string>
#include <vector>

BattleStatBattleState::BattleState() {
    party.emplace_back();  // first PC for now

    party[0].stats.attack = 2;
    party[0].stats.defence = 1;
    party[0].stats.max_hp = 10;
    party[0].hp = party[0].stats.max_hp;

    enemy.stats.attack = 4;
    enemy.stats.defence = 1;
    enemy.stats.max_hp = 10;
    enemy.hp = enemy.stats.max_hp;
}ttleState::on_enter(Game&) {
    combat_log_.emplace_back("Woe, a fiend is upon ye!");
}

void BattleState::render(const Game&) const {
    // For each of the message vectors
    for (const auto* message_vector :
         {&combat_log_, &status_display_, &action_menu_}) {
        // For each message in the current message vector
        for (const auto& message : *message_vector) {
            std::print("{}", message); // Print it
        }
    }
}

void BattleState::handle_input(Game&, std::string_view input) {
    const std::string choice = normalize_input(input);

    clear_message_vectors();

    // If for some reason the battle started, but it's already over.
    if (!in_battle || !pc.is_alive() || !enemy.is_alive()) {
        combat_log_.emplace_back("The battle is already over.\n");
        return;
    }

    if (choice == "1" || choice == "attack") {
        damage_dealt = pc.attack(enemy);
        combat_log_.emplace_back(
            std::format("You hit the enemy and dealt {} damage.\n",
                        std::to_string(damage_dealt)));

        if (!enemy.is_alive()) {
            in_battle = false;
            combat_log_.emplace_back("You emerge victorious!\n");
        }
    } else if (choice == "2" || choice == "flee") {
        in_battle = false;
        combat_log_.emplace_back("You flee... a coward's choice.\n");
        // TODO: Add the line:
        // game.request_state_change(std::make_unique<ExploreState>)
    } else {
        combat_log_.emplace_back("Invalid choice.\n");
    }

    // Basic attack from enemy.
    if (in_battle && enemy.is_alive()) {
        damage_dealt = enemy.attack(pc);
        combat_log_.emplace_back(std::format("You were hit for {} damage.\n",
                                             std::to_string(damage_dealt)));
        // YOU DIED
        if (!pc.is_alive()) {
            in_battle = false;
            combat_log_.emplace_back("You have died... shameful display!\n");
            // TODO: Add the line:
            // game.request_state_change(std::make_unique<GameOverState>)
        }
    }

    // Update the status display
    if (in_battle) {
        status_display_.emplace_back(
            std::format("Your HP: {}/{}", pc.get_hp(), pc.get_stats().max_hp));
        status_display_.emplace_back(std::format(
            "Enemy HP: {}/{}", enemy.get_hp(), enemy.get_stats().max_hp));
void BattleState::clear_message_vectors() {
    combat_log_.clear();
    status_display_.clear();
    action_menu_.clear();
}
