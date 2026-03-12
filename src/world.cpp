#include "world.hpp"
#include "enemy.hpp"
#include "player_character.hpp"
#include "stats.hpp"
#include <format>
#include <memory>

const int_fast8_t WORLD_HEIGHT = 9;
const int_fast8_t WORLD_WIDTH = 9;

void World::reset_new_game() {
    auto initial_stats = Stats{8, 10, 12};
    auto player_ =
        PlayerCharacter{"The Knight", initial_stats, initial_stats.max_hp};
    for (int i = 0; i < 3; i++) {
        std::string new_enemy_name = std::format("Silly Slime {}", i);
        auto new_enemy_stats = Stats{6 + i, 8 + i, 10 + i};
        std::unique_ptr<Enemy> new_enemy = std::make_unique<Enemy>(
            new_enemy_name, new_enemy_stats, new_enemy_stats.max_hp);
        enemies_.push_back(std::move(new_enemy));
    }
}

const PlayerCharacter& World::get_player() const noexcept {
    return player_;
}

const std::vector<std::unique_ptr<Enemy>>& World::get_enemies() const noexcept {
    return enemies_;
}

void World::populate_overworld() {
    move_entity(&player_, player_.get_x_pos(), player_.get_y_pos());
    for (const auto& enemy : enemies_) {
        move_entity(enemy.get(), enemy->get_x_pos(), enemy->get_y_pos());
    }
}

void World::move_entity(Entity* entity, int_fast8_t new_x_pos,
                        int_fast8_t new_y_pos) {
    // If the new position is out of bounds, return.
    if (new_x_pos < 0 || new_x_pos > WORLD_WIDTH || new_y_pos < 0 ||
        new_y_pos > WORLD_HEIGHT) {
        return;
    }

    // If the tile isn't empty, return.
    if (overworld_occupants_[new_x_pos][new_y_pos] != nullptr) {
        return;
    }

    // Move the entity to the new position.
    overworld_occupants_[new_x_pos][new_y_pos] = entity;
    return;
}
