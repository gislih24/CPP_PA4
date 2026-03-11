#include "enemy.cpp"
#include "game_state.cpp"
#include "player_character.cpp"
#include <iostream>
#include <print>
#include <string>

class BattleState : public GameState {
  public:
    PlayerCharacter pc = PlayerCharacter{};
    Enemy enemy = Enemy{};

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

    std::string line;
    bool in_battle = true;
    int damage_dealt;

    void render() override {
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

    void handle_input() override {}

    void update() override {}
};
