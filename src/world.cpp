#include "./include/world.hpp"
#include "./include/enemy.hpp"
#include "./include/player_character.hpp"
#include "./include/stats.hpp"
#include <algorithm>
#include <format>
#include <inttypes.h>
#include <memory>

namespace {
constexpr int WORLD_HEIGHT = 5;
constexpr int WORLD_WIDTH = 5;

std::size_t to_index(int value) {
    return static_cast<std::size_t>(value);
}
} // namespace

World::World()
    : player_("The Knight", Stats{0, 0, 0}, 0),
      overworld_occupants_(WORLD_HEIGHT,
                           std::vector<Entity*>(WORLD_WIDTH, nullptr)) {}

void World::reset_new_game() {
    defeated_enemies_ = 0;
    enemies_.clear();
    for (auto& row : overworld_occupants_) {
        std::ranges::fill(row, nullptr);
    }

    auto initial_stats = Stats{20, 6, 2};
    player_ =
        PlayerCharacter{"The Knight", initial_stats, initial_stats.max_hp};
    player_.set_position(0, 0);
    for (int i = 0; i < 3; i++) {
        std::string new_enemy_name = std::format("Silly Slime {}", i);
        auto new_enemy_stats = Stats{6 + (i * 2), 3 + i, 1 + i};
        auto new_enemy = std::make_unique<Enemy>(
            new_enemy_name, new_enemy_stats, new_enemy_stats.max_hp);
        new_enemy->set_position(i + 1, i + 1);
        enemies_.push_back(std::move(new_enemy));
    }
    populate_overworld();
}

PlayerCharacter& World::get_player() noexcept {
    return player_;
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

void World::move_entity(Entity* entity, int new_x_pos, int new_y_pos) noexcept {
    // If the new position is out of bounds, return.
    if (new_x_pos < 0 || new_x_pos >= WORLD_WIDTH || new_y_pos < 0 ||
        new_y_pos >= WORLD_HEIGHT) {
        return;
    }

    // If the tile isn't empty, return.
    if (overworld_occupants_[new_x_pos][new_y_pos] != nullptr) {
        return;
    }

    // Move the entity to the new position.
    overworld_occupants_[new_x_pos][new_y_pos] = entity;
}
