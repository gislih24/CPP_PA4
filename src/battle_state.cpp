#include "./include/battle_state.hpp"
#include "./include/explore_state.hpp"
#include "./include/game.hpp"
#include <format>
#include <iostream>
#include <memory>
#include <print>

BattleState::BattleState(Enemy* enemy)
    : enemy_(enemy),
      encounter_position_(
          enemy == nullptr ? Position{}
                           : Position{enemy->get_x_pos(), enemy->get_y_pos()}),
      enemy_name_(enemy == nullptr ? "Unknown enemy"
                                   : std::string(enemy->get_name())) {}

void BattleState::on_enter(Game& game) {
    clear_message_vectors();

    if (enemy_ == nullptr) {
        combat_log_.emplace_back("There is nothing here to fight.\n");
        outcome_ = Outcome::Fled;
        rebuild_status(game);
        return;
    }

    combat_log_.emplace_back(
        std::format("A devious {} blocks your path!\n", enemy_name_));
    rebuild_status(game);
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

    std::cout << std::flush;
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
        action_menu_.emplace_back("Choose an action:\n1. attack\n2. flee\n");
    }
}

void BattleState::clear_message_vectors() {
    combat_log_.clear();
    status_display_.clear();
    action_menu_.clear();
}

void BattleState::rebuild_status(const Game& game) {
    const auto& pc = game.get_world().get_player();
    const int enemy_hp = enemy_ == nullptr ? 0 : enemy_->get_hp();
    const int enemy_max_hp = enemy_ == nullptr ? 0 : enemy_->get_stats().max_hp;

    status_display_.emplace_back(
        std::format("Your HP: {}/{}\n", pc.get_hp(), pc.get_stats().max_hp));
    status_display_.emplace_back(
        std::format("{} HP: {}/{}\n", enemy_name_, enemy_hp, enemy_max_hp));

    switch (outcome_) {
    case Outcome::Ongoing:
        action_menu_.emplace_back("Choose an action:\n1. attack\n2. flee\n");
        break;
    case Outcome::Victory:
        action_menu_.emplace_back("Press enter to return to the overworld.\n");
        break;
    case Outcome::Fled:
        action_menu_.emplace_back("Press enter to return to the overworld.\n");
        break;
    case Outcome::Defeat:
        action_menu_.emplace_back("Press enter to end the run.\n");
        break;
    }

    action_menu_.emplace_back("> ");
}

void BattleState::leave_battle(Game& game) {
    switch (outcome_) {
    case Outcome::Victory: {
        game.get_world().remove_enemy(enemy_);
        game.get_world().set_player_position(encounter_position_);
        game.request_state_change(std::make_unique<ExploreState>(
            std::format("You defeated {}.", enemy_name_)));
        return;
    }
    case Outcome::Fled:
        game.request_state_change(std::make_unique<ExploreState>(
            "You retreat to your previous tile."));
        return;
    case Outcome::Defeat:
        game.quit();
        return;
    case Outcome::Ongoing:
        return;
    }
}
