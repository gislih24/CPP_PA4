#include "./include/battle_state.hpp"
#include "./include/character_classes.hpp"
#include "./include/enemy.hpp"
#include "./include/game_state.hpp"
#include "./include/player_character.hpp"
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

BattleState::BattleState() {
    party.add_member(std::make_unique<Knight>());
    party.add_member(std::make_unique<Wizard>());
    //set stats for each party member
    party.members()[0]->set_attack(3);
    party.members()[0]->set_defence(2);
    party.members()[0]->set_max_hp(14);
    party.members()[0]->set_hp(14);
    
    party.members()[1]->set_attack(5);
    party.members()[1]->set_defence(0);
    party.members()[1]->set_max_hp(8);
    party.members()[1]->set_hp(8);

    

    enemy.set_attack(4);
    enemy.set_defence(1);
    enemy.set_max_hp(10);
    enemy.set_hp(10);
}

void BattleState::on_enter(Game&) {
    combat_log_.emplace_back("Woe, a fiend is upon ye!\n");

    // Show initial action options only for the first alive party member so
    // the player sees that character's menu on first render.
    const auto& members = party.members();
    for (const auto& member_ptr : members) {
        if (!member_ptr || !member_ptr->is_alive()) {
            continue;
        }

        std::string class_name = "Adventurer";
        std::string ability_1 = "none";
        std::string ability_2 = "none";
        if (dynamic_cast<Knight*>(member_ptr.get()) != nullptr) {
            class_name = "Knight";
            ability_1 = "shield_brace";
            ability_2 = "dance";
        } else if (dynamic_cast<Wizard*>(member_ptr.get()) != nullptr) {
            class_name = "Wizard";
            ability_1 = "fireball";
            ability_2 = "heal";
        }

        combat_log_.emplace_back(
            class_name +
            "'s turn. Choose your action.\n1. attack\n2. " + ability_1 +
            "\n3. " + ability_2 + "\n0. flee.\n");
        break;
    }
}

void BattleState::render(const Game&) const {
    // For each of the message vectors
    for (const auto* message_vector :
         {&combat_log_, &status_display_, &action_menu_}) {
        // For each message in the current message vector
        for (const auto& message : *message_vector) {
            std::cout << message; // Print it
        }
    }
    // Prompt for the player's input, similar to MainMenuState.
    std::cout << "> " << std::flush;
}

void BattleState::handle_input(Game&, std::string_view input) {
    std::string choice = normalize_input(input);

    clear_message_vectors();

    if (party.members().empty()) {
        in_battle = false;
        combat_log_.emplace_back("Your party is empty.\n");
        return;
    }

    bool any_party_member_alive = false;
    for (const auto& member_ptr : party.members()) {
        if (member_ptr && member_ptr->is_alive()) {
            any_party_member_alive = true;
            break;
        }
    }

    if (!in_battle || !any_party_member_alive || !enemy.is_alive()) {
        combat_log_.emplace_back("The battle is already over.\n");
        return;
    }

    bool player_fled = false;
    // Find the current acting party member starting from current_actor_index.
    PlayerCharacter* actor = nullptr;
    std::size_t actor_index = current_actor_index;
    auto& members = party.members();
    while (actor_index < members.size()) {
        if (members[actor_index] && members[actor_index]->is_alive()) {
            actor = members[actor_index].get();
            break;
        }
        ++actor_index;
    }

    if (!actor) {
        // No alive party members left.
        in_battle = false;
        combat_log_.emplace_back(
            "Your party has been defeated... shameful display!\n");
        return;
    }

    PlayerCharacter& pc = *actor;

    std::string class_name = "Adventurer";
    std::string ability_1 = "none";
    std::string ability_2 = "none";
    if (dynamic_cast<Knight*>(actor) != nullptr) {
        class_name = "Knight";
        ability_1 = "shield_brace";
        ability_2 = "dance";
    } else if (dynamic_cast<Wizard*>(actor) != nullptr) {
        class_name = "Wizard";
        ability_1 = "fireball";
        ability_2 = "heal";
    }

    auto append_menu_for = [&](PlayerCharacter* who, const std::string& cls,
                               const std::string& a1,
                               const std::string& a2) {
        (void)who; // currently unused but kept for future extension
        combat_log_.emplace_back(cls +
                                 "'s turn. Choose your action.\n1. attack\n2. " +
                                 a1 + "\n3. " + a2 + "\n0. flee.\n");
    };

    if (choice.empty()) {
        append_menu_for(actor, class_name, ability_1, ability_2);
        return;
    }

    if (choice == "1" || choice == "attack") {
        damage_dealt = pc.attack(enemy);
        combat_log_.emplace_back(class_name + " attacks and deals " +
                                 std::to_string(damage_dealt) +
                                 " damage.\n");
    } else if (choice == "2") {
        if (auto* knight = dynamic_cast<Knight*>(actor)) {
            if (!shield_brace_active) {
                for (auto& ally_ptr : party.members()) {
                    if (!ally_ptr) {
                        continue;
                    }
                    const int current_def =
                        ally_ptr->get_stats().defence;
                    ally_ptr->set_defence(current_def + 2);
                }
                shield_brace_active = true;
                knight->shield_brace();
                combat_log_.emplace_back(
                    "Knight uses shield_brace. Allies' defence rises by 2 until next turn.\n");
            } else {
                combat_log_.emplace_back(
                    "Shield brace is already active.\n");
            }
        } else if (auto* wizard = dynamic_cast<Wizard*>(actor)) {
            damage_dealt = wizard->fireball(enemy); // TODO: make fireball damage all enemies
            combat_log_.emplace_back(
                "Wizard casts fireball and deals " +
                std::to_string(damage_dealt) + " damage.\n");
            // TODO: account for multiple enemies
        } else {
            combat_log_.emplace_back("No ability in slot 2.\n");
        }
    } else if (choice == "3") {
        if (auto* wizard = dynamic_cast<Wizard*>(actor)) {
            // Prompt the player to choose which ally to heal.
            combat_log_.emplace_back(
                "Choose an ally to heal (enter number):\n");
            for (std::size_t i = 0; i < party.members().size(); ++i) {
                const auto& ally = party.members()[i];
                if (!ally || !ally->is_alive()) {
                    continue;
                }

                std::string ally_label = "Ally";
                if (dynamic_cast<Knight*>(ally.get()) != nullptr) {
                    ally_label = "Knight";
                } else if (dynamic_cast<Wizard*>(ally.get()) != nullptr) {
                    ally_label = "Wizard";
                }

                combat_log_.emplace_back(
                    std::to_string(i + 1) + ". " + ally_label +
                    " HP: " + std::to_string(ally->get_hp()) + "/" +
                    std::to_string(ally->get_stats().max_hp) + "\n");
            }

            // Print the accumulated messages so the prompt shows
            // before we read from stdin.
            for (const auto* message_vector :
                 {&combat_log_, &status_display_, &action_menu_}) {
                for (const auto& message : *message_vector) {
                    std::cout << message;
                }
            }

            std::string target_input;
            if (!std::getline(std::cin, target_input)) {
                combat_log_.emplace_back(
                    "No target selected for healing.\n");
            } else {
                int idx = -1;
                try {
                    idx = std::stoi(target_input);
                } catch (...) {
                    idx = -1;
                }

                if (idx <= 0 ||
                    static_cast<std::size_t>(idx) >
                        party.members().size() ||
                    !party.members()[static_cast<std::size_t>(idx) - 1] ||
                    !party.members()[static_cast<std::size_t>(idx) - 1]
                         ->is_alive()) {
                    combat_log_.emplace_back(
                        "Invalid ally selection. Healing fails.\n");
                } else {
                    auto& target_ptr =
                        party.members()[static_cast<std::size_t>(idx) - 1];
                    auto& target_pc = *target_ptr;
                    wizard->healing_touch(target_pc);
                    combat_log_.emplace_back(
                        "Wizard casts healing touch on ally " +
                        std::to_string(idx) + ".\n");
                }
            }
        } else if (auto* knight = dynamic_cast<Knight*>(actor)) {
            combat_log_.emplace_back("Knight dances...\n");
        } else {
            combat_log_.emplace_back("No ability in slot 3.\n");
        }
    } else if (choice == "0" || choice == "flee") {
        in_battle = false;
        player_fled = true;
        combat_log_.emplace_back("You flee... a coward's choice.\n");
    } else {
        combat_log_.emplace_back("Invalid choice.\n");
    }

    if (!enemy.is_alive()) {
        in_battle = false;
        combat_log_.emplace_back("You emerge victorious!\n");
        return;
    }

    // If there are more alive party members after this one, advance the
    // actor index and prepare the next actor's menu. Enemy waits.
    std::size_t next_index = actor_index + 1;
    PlayerCharacter* next_actor = nullptr;
    while (next_index < members.size()) {
        if (members[next_index] && members[next_index]->is_alive()) {
            next_actor = members[next_index].get();
            break;
        }
        ++next_index;
    }

    if (in_battle && enemy.is_alive() && !player_fled && next_actor) {
        current_actor_index = next_index;

        // Prepare the next actor's menu so it is visible on the next render.
        std::string next_class = "Adventurer";
        std::string next_a1 = "none";
        std::string next_a2 = "none";
        if (dynamic_cast<Knight*>(next_actor) != nullptr) {
            next_class = "Knight";
            next_a1 = "shield_brace";
            next_a2 = "dance";
        } else if (dynamic_cast<Wizard*>(next_actor) != nullptr) {
            next_class = "Wizard";
            next_a1 = "fireball";
            next_a2 = "heal";
        }

        append_menu_for(next_actor, next_class, next_a1, next_a2);
        return;
    }

    // No more player characters this round; enemy takes its turn.
    current_actor_index = 0;

    if (in_battle && enemy.is_alive() && !player_fled) {
        // Choose a random alive party member as the target.
        std::vector<PlayerCharacter*> alive_members;
        for (auto& member_ptr : party.members()) {
            if (member_ptr && member_ptr->is_alive()) {
                alive_members.push_back(member_ptr.get());
            }
        }

        if (alive_members.empty()) {
            in_battle = false;
            combat_log_.emplace_back("Your party has been defeated... shameful display!\n");
            return;
        }

        // Simple pseudo-random selection each enemy turn.
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<std::size_t> dist(0, alive_members.size() - 1);
        PlayerCharacter* target = alive_members[dist(rng)];

        std::string target_label = "Ally";
        if (dynamic_cast<Knight*>(target) != nullptr) {
            target_label = "Knight";
        } else if (dynamic_cast<Wizard*>(target) != nullptr) {
            target_label = "Wizard";
        }

        damage_dealt = enemy.attack(*target);
        combat_log_.emplace_back("\n" + target_label + " was hit for " +
                     std::to_string(damage_dealt) +
                     " damage.\n\n");

        // Shield brace bonus ends after the enemy has taken its turn.
        if (shield_brace_active) {
            for (auto& member_ptr : party.members()) {
                if (!member_ptr) {
                    continue;
                }
                const int current_def = member_ptr->get_stats().defence;
                member_ptr->set_defence(current_def - 2);
            }
            shield_brace_active = false;
        }

        bool party_still_alive = false;
        for (const auto& member_ptr : party.members()) {
            if (member_ptr && member_ptr->is_alive()) {
                party_still_alive = true;
                break;
            }
        }

        if (!party_still_alive) {
            in_battle = false;
            combat_log_.emplace_back("Your party has been defeated... shameful display!\n");
            // TODO: Add the line:
            // game.request_state_change(std::make_unique<GameOverState>)
        }
    }

    if (in_battle) {
        status_display_.emplace_back("\n");
        for (std::size_t index = 0; index < party.members().size(); ++index) {
            const auto& member_ptr = party.members()[index];
            if (!member_ptr) {
                continue;
            }

            std::string member_label = "Ally";
            if (dynamic_cast<Knight*>(member_ptr.get()) != nullptr) {
                member_label = "Knight";
            } else if (dynamic_cast<Wizard*>(member_ptr.get()) != nullptr) {
                member_label = "Wizard";
            }

            status_display_.emplace_back(
                member_label + " HP: " +
                std::to_string(member_ptr->get_hp()) + "/" +
                std::to_string(member_ptr->get_stats().max_hp) + "\n");
        }
        status_display_.emplace_back("Enemy HP: " +
                                     std::to_string(enemy.get_hp()) + "/" +
                                     std::to_string(
                                         enemy.get_stats().max_hp) +
                                     "\n\n");

        // Prepare the next round's first actor menu so the player
        // doesn't need to press Enter just to see it.
        const auto& members_for_next = party.members();
        for (const auto& member_ptr : members_for_next) {
            if (!member_ptr || !member_ptr->is_alive()) {
                continue;
            }

            std::string next_class = "Adventurer";
            std::string next_a1 = "none";
            std::string next_a2 = "none";
            if (dynamic_cast<Knight*>(member_ptr.get()) != nullptr) {
                next_class = "Knight";
                next_a1 = "shield_brace";
                next_a2 = "dance";
            } else if (dynamic_cast<Wizard*>(member_ptr.get()) != nullptr) {
                next_class = "Wizard";
                next_a1 = "fireball";
                next_a2 = "heal";
            }

            action_menu_.emplace_back(
                next_class +
                "'s turn. Choose your action.\n1. attack\n2. " + next_a1 +
                "\n3. " + next_a2 + "\n0. flee.\n");
            break; // only the first alive member
        }
    }
}

void BattleState::clear_message_vectors() {
    combat_log_.clear();
    status_display_.clear();
    action_menu_.clear();
}
