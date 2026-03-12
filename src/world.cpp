#include "world.hpp"
#include "enemy.hpp"
#include "player_character.hpp"
#include "stats.hpp"
#include <format>
#include <memory>

void World::reset_new_game() const {
    auto initial_stats = Stats{8, 10, 12};
    auto player_ =
        PlayerCharacter{"The Knight", initial_stats, initial_stats.max_hp};
    for (int i = 0; i < 3; i++) {
        std::string new_enemy_name = std::format("Silly Slime {}", i);
        auto new_enemy_stats = Stats{6 + i, 8 + i, 10 + i};
        std::unique_ptr<Enemy> new_enemy = std::make_unique<Enemy>(
            new_enemy_name, new_enemy_stats, new_enemy_stats.max_hp);
    }
}

const PlayerCharacter& World::get_player() const noexcept {
    return player_;
}