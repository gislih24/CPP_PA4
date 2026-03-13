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

const std::vector<std::vector<Entity*>>&
World::get_overworld_occupants() const noexcept {
    return overworld_occupants_;
}

int World::defeated_enemies() const noexcept {
    return defeated_enemies_;
}

MoveOutcome World::try_move_player(int row_change, int col_change) noexcept {
    const Position current{player_.get_x_pos(), player_.get_y_pos()};
    const Position destination{
        current.row + row_change,
        current.col + col_change,
    };

    if (!is_in_bounds(destination)) {
        return {.result = MoveResult::out_of_bounds,
                .destination = destination,
                .enemy = nullptr};
    }

    Entity* occupant = get_occupant_at(destination);
    if (occupant == nullptr) {
        set_player_position(destination);
        return {.result = MoveResult::moved,
                .destination = destination,
                .enemy = nullptr};
    }

    if (auto* enemy = dynamic_cast<Enemy*>(occupant)) {
        return {.result = MoveResult::enemy_encounter,
                .destination = destination,
                .enemy = enemy};
    }

    return {.result = MoveResult::blocked,
            .destination = destination,
            .enemy = nullptr};
}

void World::set_player_position(Position position) noexcept {
    if (!is_in_bounds(position)) {
        return;
    }

    if (Entity const* dest_occupant = get_occupant_at(position);
        dest_occupant != nullptr && dest_occupant != &player_) {
        return;
    }

    if (const Position current_pos{player_.get_x_pos(), player_.get_y_pos()};
        is_in_bounds(current_pos) && get_occupant_at(current_pos) == &player_) {
        clear_tile(current_pos);
    }

    set_tile(position, &player_);
    player_.set_position(int(position.row), int(position.col));
}

bool World::remove_enemy(Enemy const* enemy) noexcept {
    if (enemy == nullptr) {
        return false;
    }

    if (Position enemy_position{enemy->get_x_pos(), enemy->get_y_pos()};
        is_in_bounds(enemy_position) &&
        get_occupant_at(enemy_position) == enemy) {
        clear_tile(enemy_position);
    }

    const auto removed = std::erase_if(
        enemies_, [enemy](const std::unique_ptr<Enemy>& candidate) {
            return candidate.get() == enemy;
        });

    defeated_enemies_ += static_cast<int>(removed);
    return removed > 0;
}

void World::populate_overworld() {
    move_entity(&player_, player_.get_x_pos(), player_.get_y_pos());
    for (const auto& enemy : enemies_) {
        move_entity(enemy.get(), enemy->get_x_pos(), enemy->get_y_pos());
    }
}

void World::move_entity(Entity* entity, int new_x_pos, int new_y_pos) noexcept {
    if (entity == nullptr) {
        return;
    }

    // If the new position is out of bounds, return.
    if (!is_in_bounds({new_x_pos, new_y_pos})) {
        return;
    }

    // If the tile isn't empty, return.
    if (overworld_occupants_[to_index(new_x_pos)][to_index(new_y_pos)] !=
        nullptr) {
        return;
    }

    // Move the entity to the new position.
    set_tile({new_x_pos, new_y_pos}, entity);
    entity->set_position(new_x_pos, new_y_pos);
}

bool World::is_in_bounds(Position position) const noexcept {
    return position.row >= 0 && position.row < WORLD_HEIGHT &&
           position.col >= 0 && position.col < WORLD_WIDTH;
}

Entity* World::get_occupant_at(Position position) const noexcept {
    if (!is_in_bounds(position)) {
        return nullptr;
    }

    return overworld_occupants_[to_index(position.row)][to_index(position.col)];
}

void World::clear_tile(Position position) noexcept {
    if (!is_in_bounds(position)) {
        return;
    }

    overworld_occupants_[to_index(position.row)][to_index(position.col)] =
        nullptr;
}

void World::set_tile(Position position, Entity* entity) noexcept {
    if (!is_in_bounds(position)) {
        return;
    }

    overworld_occupants_[to_index(position.row)][to_index(position.col)] =
        entity;
}
