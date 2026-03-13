#include "./include/battle_state.hpp"
#include "./include/character_classes.hpp"
#include "./include/explore_state.hpp"
#include "./include/game.hpp"
#include <format>
#include <iostream>
#include <memory>
#include <random>
#include <string_view>
#include <utility>
#include <vector>

namespace {

std::pair<PlayerCharacter*, std::size_t>
find_next_alive_member(Party& party, std::size_t start_index) {
    auto& members = party.members();
    for (std::size_t index = start_index; index < members.size(); ++index) {
        if (members[index] != nullptr && members[index]->is_alive()) {
            return {members[index].get(), index};
        }
    }

    return {nullptr, members.size()};
}

bool any_party_members_alive(const Party& party) {
    for (const auto& member : party.members()) {
        if (member != nullptr && member->is_alive()) {
            return true;
        }
    }

    return false;
}

std::string_view role_label(const PlayerCharacter& actor) {
    if (dynamic_cast<const Knight*>(&actor) != nullptr) {
        return "Knight";
    }
    if (dynamic_cast<const Wizard*>(&actor) != nullptr) {
        return "Wizard";
    }

    return "Adventurer";
}

std::pair<std::string_view, std::string_view>
role_abilities(const PlayerCharacter& actor) {
    if (dynamic_cast<const Knight*>(&actor) != nullptr) {
        return {"shield_brace", "dance"};
    }
    if (dynamic_cast<const Wizard*>(&actor) != nullptr) {
        return {"fireball", "heal"};
    }

    return {"none", "none"};
}

} // namespace

BattleState::BattleState(Enemy* enemy)
    : enemy_(enemy),
      encounter_position_(
          enemy == nullptr ? Position{}
                           : Position{enemy->get_x_pos(), enemy->get_y_pos()}),
      enemy_name_(enemy == nullptr ? "Unknown enemy"
                                   : std::string(enemy->get_name())) {}

void BattleState::on_enter(Game& game) {
    clear_message_vectors();
    initialize_party(game);

    if (enemy_ == nullptr) {
        combat_log_.emplace_back("There is nothing here to fight.\n");
        outcome_ = Outcome::Fled;
        rebuild_status();
        return;
    }

    if (!enemy_->is_alive()) {
        combat_log_.emplace_back("The battle is already over.\n");
        outcome_ = Outcome::Victory;
        rebuild_status();
        return;
    }

    combat_log_.emplace_back(std::format(
        "Woe, a devious {} fiend blocks your path!\n", enemy_name_));
    rebuild_status();
}

void BattleState::render(const Game&) const {
    for (const auto* message_vector :
         {&combat_log_, &status_display_, &action_menu_}) {
        for (const auto& message : *message_vector) {
            std::cout << message;
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

    if (enemy_ == nullptr || !enemy_->is_alive()) {
        combat_log_.emplace_back("The battle is already over.\n");
        outcome_ = Outcome::Victory;
        rebuild_status();
        return;
    }

    auto& members = party_.members();
    if (members.empty()) {
        combat_log_.emplace_back("Your party is empty.\n");
        outcome_ = Outcome::Defeat;
        rebuild_status();
        return;
    }

    if (members.front() == nullptr || !members.front()->is_alive()) {
        combat_log_.emplace_back(
            "The Knight falls. The adventure ends here.\n");
        outcome_ = Outcome::Defeat;
        sync_party_leader(game);
        rebuild_status();
        return;
    }

    if (!any_party_members_alive(party_)) {
        combat_log_.emplace_back("Your party has been defeated.\n");
        outcome_ = Outcome::Defeat;
        sync_party_leader(game);
        rebuild_status();
        return;
    }

    auto [actor, actor_index] =
        find_next_alive_member(party_, current_actor_index_);
    if (actor == nullptr) {
        current_actor_index_ = 0;
        std::tie(actor, actor_index) = find_next_alive_member(party_, 0);
    }

    if (actor == nullptr) {
        combat_log_.emplace_back("Your party has been defeated.\n");
        outcome_ = Outcome::Defeat;
        sync_party_leader(game);
        rebuild_status();
        return;
    }

    if (awaiting_heal_target_) {
        auto* wizard = dynamic_cast<Wizard*>(actor);
        if (wizard == nullptr) {
            awaiting_heal_target_ = false;
            rebuild_status();
            return;
        }

        if (choice.empty()) {
            rebuild_status();
            return;
        }

        if (choice == "0" || choice == "cancel") {
            awaiting_heal_target_ = false;
            combat_log_.emplace_back("Healing cancelled.\n");
            rebuild_status();
            return;
        }

        int selected_index = -1;
        try {
            selected_index = std::stoi(choice);
        } catch (const std::invalid_argument&) {
            selected_index = -1;
        }

        if (selected_index <= 0 ||
            static_cast<std::size_t>(selected_index) > members.size() ||
            members[static_cast<std::size_t>(selected_index) - 1] == nullptr ||
            !members[static_cast<std::size_t>(selected_index) - 1]
                 ->is_alive()) {
            combat_log_.emplace_back("Invalid ally selection.\n");
            rebuild_status();
            return;
        }

        auto& target = *members[static_cast<std::size_t>(selected_index) - 1];
        wizard->healing_touch(target);
        awaiting_heal_target_ = false;
        combat_log_.emplace_back(std::format(
            "Wizard casts healing touch on {}.\n", role_label(target)));
        sync_party_leader(game);
    } else {
        if (choice.empty()) {
            rebuild_status();
            return;
        }

        const auto [ability_1, ability_2] = role_abilities(*actor);

        if (choice == "1" || choice == "attack") {
            const int damage_dealt = actor->attack(*enemy_);
            combat_log_.emplace_back(
                std::format("{} attacks and deals {} damage.\n",
                            role_label(*actor), damage_dealt));
        } else if (choice == "2" || choice == ability_1) {
            if (auto* knight = dynamic_cast<Knight*>(actor)) {
                if (shield_brace_active_) {
                    combat_log_.emplace_back(
                        "Shield brace is already active.\n");
                    rebuild_status();
                    return;
                }

                for (auto const& ally : members) {
                    if (ally != nullptr) {
                        ally->set_defence(ally->get_stats().defence + 2);
                    }
                }
                shield_brace_active_ = true;
                knight->shield_brace();
                combat_log_.emplace_back(
                    "Knight uses shield_brace. Allies gain 2 defence until the "
                    "enemy acts.\n");
            } else if (auto* wizard = dynamic_cast<Wizard*>(actor)) {
                const int damage_dealt = wizard->fireball(*enemy_);
                combat_log_.emplace_back(
                    std::format("Wizard casts fireball and deals {} damage.\n",
                                damage_dealt));
            } else {
                combat_log_.emplace_back("No ability is bound to slot 2.\n");
                rebuild_status();
                return;
            }
        } else if (choice == "3" || choice == ability_2) {
            if (dynamic_cast<Wizard*>(actor) != nullptr) {
                awaiting_heal_target_ = true;
                rebuild_status();
                return;
            }

            if (dynamic_cast<Knight*>(actor) != nullptr) {
                combat_log_.emplace_back(
                    "Knight dances. Morale rises, but nothing else happens.\n");
            } else {
                combat_log_.emplace_back("No ability is bound to slot 3.\n");
                rebuild_status();
                return;
            }
        } else if (choice == "0" || choice == "flee") {
            combat_log_.emplace_back("You flee... a coward's choice.\n");
            outcome_ = Outcome::Fled;
            sync_party_leader(game);
            rebuild_status();
            return;
        } else {
            combat_log_.emplace_back("Invalid choice.\n");
            rebuild_status();
            return;
        }
    }

    if (!members.front()->is_alive()) {
        combat_log_.emplace_back(
            "The Knight falls. The adventure ends here.\n");
        outcome_ = Outcome::Defeat;
        sync_party_leader(game);
        rebuild_status();
        return;
    }

    if (!enemy_->is_alive()) {
        combat_log_.emplace_back("You emerge victorious!\n");
        outcome_ = Outcome::Victory;
        sync_party_leader(game);
        rebuild_status();
        return;
    }

    auto [next_actor, next_index] =
        find_next_alive_member(party_, actor_index + 1);
    if (next_actor != nullptr) {
        current_actor_index_ = next_index;
        sync_party_leader(game);
        rebuild_status();
        return;
    }

    current_actor_index_ = 0;

    std::vector<PlayerCharacter*> alive_members;
    alive_members.reserve(members.size());
    for (auto const& member : members) {
        if (member != nullptr && member->is_alive()) {
            alive_members.push_back(member.get());
        }
    }

    if (alive_members.empty()) {
        combat_log_.emplace_back("Your party has been defeated.\n");
        outcome_ = Outcome::Defeat;
        sync_party_leader(game);
        rebuild_status();
        return;
    }

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0,
                                                    alive_members.size() - 1);
    PlayerCharacter* target = alive_members[dist(rng)];

    const int damage_dealt = enemy_->attack(*target);
    combat_log_.emplace_back(std::format("{} hits {} for {} damage.\n",
                                         enemy_name_, role_label(*target),
                                         damage_dealt));

    if (shield_brace_active_) {
        for (auto const& member : members) {
            if (member != nullptr) {
                member->set_defence(member->get_stats().defence - 2);
            }
        }
        shield_brace_active_ = false;
        combat_log_.emplace_back("Shield brace wears off.\n");
    }

    if (!members.front()->is_alive()) {
        combat_log_.emplace_back(
            "The Knight falls. The adventure ends here.\n");
        outcome_ = Outcome::Defeat;
        sync_party_leader(game);
        rebuild_status();
        return;
    }

    if (!any_party_members_alive(party_)) {
        combat_log_.emplace_back("Your party has been defeated.\n");
        outcome_ = Outcome::Defeat;
        sync_party_leader(game);
        rebuild_status();
        return;
    }

    sync_party_leader(game);
    rebuild_status();
}

void BattleState::clear_message_vectors() {
    combat_log_.clear();
    status_display_.clear();
    action_menu_.clear();
}

void BattleState::initialize_party(const Game& game) {
    if (!party_.members().empty()) {
        return;
    }

    const auto& player = game.get_world().get_player();
    party_.add_member(
        std::make_unique<Knight>(player.get_stats(), player.get_hp()));
    party_.add_member(std::make_unique<Wizard>());
}

void BattleState::rebuild_status() {
    status_display_.emplace_back("Party status\n");
    for (const auto& member : party_.members()) {
        if (member == nullptr) {
            continue;
        }

        status_display_.emplace_back(
            std::format("{} HP: {}/{}\n", role_label(*member), member->get_hp(),
                        member->get_stats().max_hp));
    }

    const int enemy_hp = enemy_ == nullptr ? 0 : enemy_->get_hp();
    const int enemy_max_hp = enemy_ == nullptr ? 0 : enemy_->get_stats().max_hp;
    status_display_.emplace_back(
        std::format("{} HP: {}/{}\n", enemy_name_, enemy_hp, enemy_max_hp));

    switch (outcome_) {
    case Outcome::Ongoing:
        if (awaiting_heal_target_) {
            action_menu_.emplace_back("Choose an ally to heal:\n");
            for (std::size_t index = 0; index < party_.members().size();
                 ++index) {
                const auto& member = party_.members()[index];
                if (member == nullptr || !member->is_alive()) {
                    continue;
                }

                action_menu_.emplace_back(std::format(
                    "{}. {} HP: {}/{}\n", index + 1, role_label(*member),
                    member->get_hp(), member->get_stats().max_hp));
            }
            action_menu_.emplace_back("0. cancel\n");
        } else {
            auto [actor, unused_index] =
                find_next_alive_member(party_, current_actor_index_);
            (void)unused_index;
            if (actor != nullptr) {
                const auto [ability_1, ability_2] = role_abilities(*actor);
                action_menu_.emplace_back(
                    std::format("{}'s turn. Choose an action:\n1. attack\n2. "
                                "{}\n3. {}\n0. flee\n",
                                role_label(*actor), ability_1, ability_2));
            } else {
                action_menu_.emplace_back("No one is left standing.\n");
            }
        }
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

void BattleState::sync_party_leader(Game& game) {
    if (party_.members().empty() || party_.members().front() == nullptr) {
        return;
    }

    game.get_world().get_player().set_hp(party_.members().front()->get_hp());
}

void BattleState::leave_battle(Game& game) {
    sync_party_leader(game);

    switch (outcome_) {
    case Outcome::Victory:
        game.get_world().remove_enemy(enemy_);
        game.get_world().set_player_position(encounter_position_);
        game.request_state_change(std::make_unique<ExploreState>(
            std::format("You defeated {}.", enemy_name_)));
        return;
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
