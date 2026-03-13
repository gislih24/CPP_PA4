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

void BattleState::handle_input(Game& game, std::string_view input) {
    const std::string choice = normalize_input(input);
    clear_message_vectors();

    if (outcome_ != Outcome::Ongoing) {
        leave_battle(game);
        return;
    }

    auto& pc = game.get_world().get_player();
    if (enemy_ == nullptr || !enemy_->is_alive()) {
        combat_log_.emplace_back("The battle is already over.\n");
        outcome_ = Outcome::Victory;
        rebuild_status(game);
        return;
    }

    if (choice == "1" || choice == "attack") {
        const int damage_dealt = pc.attack(*enemy_);
        combat_log_.emplace_back(std::format("You hit {} for {} damage.\n",
                                             enemy_name_, damage_dealt));

        if (!enemy_->is_alive()) {
            outcome_ = Outcome::Victory;
            combat_log_.emplace_back("You emerge victorious!\n");
            rebuild_status(game);
            return;
        }
    } else if (choice == "2" || choice == "flee") {
        outcome_ = Outcome::Fled;
        combat_log_.emplace_back("You flee... a coward's choice.\n");
        rebuild_status(game);
        return;
    } else {
        combat_log_.emplace_back("Invalid choice.\n");
        rebuild_status(game);
        return;
    }

    // Basic attack from enemy.
    const int damage_dealt = enemy_->attack(pc);
    combat_log_.emplace_back(
        std::format("{} hits you for {} damage.\n", enemy_name_, damage_dealt));
    if (!pc.is_alive()) {
        outcome_ = Outcome::Defeat;
        combat_log_.emplace_back("You have died.\n");
    }

    rebuild_status(game);
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
