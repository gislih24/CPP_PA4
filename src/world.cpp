#include "./include/world.hpp"
#include <algorithm>
#include <format>
#include <memory>
#include <ranges>

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

/**
 * @brief Resets the world to its initial state for a new game.
 *
 * This includes clearing defeated enemies count, resetting the player and
 * enemies to their initial stats and positions, and repopulating the overworld
 * with the player and enemies.
 */
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
/* ---------------------------------Getters--------------------------------- */
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

/**
 * @brief Attempts to move the player by the specified row and column changes.
 * @param row_change The change in the player's row position (positive for
 * down, negative for up).
 * @param col_change The change in the player's column position (positive for
 * down, negative for up).
 * @return A MoveOutcome struct indicating the result of the move attempt.
 */
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

/**
 * @brief Sets the player's position to the specified coordinates if the move
 * is valid.
 * @param position The new position to set for the player.
 */
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

/**
 * @brief Removes the specified enemy from the world if it exists, and updates
 * the defeated enemies count accordingly.
 * @param enemy A pointer to the enemy to be removed.
 * @return True if the enemy was successfully removed, false if the enemy was
 * not found or the pointer was null.
 */
bool World::remove_enemy(Enemy const* enemy) noexcept {
    if (enemy == nullptr) {
        return false;
    }

    int removed = 0;
    for (auto it = enemies_.begin(); it != enemies_.end();) {
        if (it->get() == enemy) {
            it = enemies_.erase(it);
            ++removed;
        } else {
            ++it;
        }
    }

    if (removed == 0) {
        return false;
    }

    if (const Position enemy_position{enemy->get_x_pos(), enemy->get_y_pos()};
        is_in_bounds(enemy_position) &&
        get_occupant_at(enemy_position) == enemy) {
        clear_tile(enemy_position);
    }

    defeated_enemies_ += removed;
    return true;
}

void World::populate_overworld() {
    move_entity(&player_, player_.get_x_pos(), player_.get_y_pos());
    for (const auto& enemy : enemies_) {
        move_entity(enemy.get(), enemy->get_x_pos(), enemy->get_y_pos());
    }
}

/**
 * @brief Moves the specified entity to the new coordinates if the move is
 * valid.
 * @param entity A pointer to the entity to be moved.
 * @param new_x_pos The new x-coordinate for the entity.
 * @param new_y_pos The new y-coordinate for the entity.
 */
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

/**
 * @brief Checks if the specified position is within the bounds of the world.
 * @param position The position to check for bounds.
 * @return True if the position is within bounds, false otherwise.
 */
bool World::is_in_bounds(Position position) const noexcept {
    return position.row >= 0 && position.row < WORLD_HEIGHT &&
           position.col >= 0 && position.col < WORLD_WIDTH;
}

/**
 * @brief Gets the entity occupying the specified position.
 * @param position The position to check.
 * @return A pointer to the entity occupying the position, or nullptr if the
 * tile is empty.
 */
Entity* World::get_occupant_at(Position position) const noexcept {
    if (!is_in_bounds(position)) {
        return nullptr;
    }

    return overworld_occupants_[to_index(position.row)][to_index(position.col)];
}

/**
 * @brief Clears the tile at the specified position by setting it to nullptr,
 * effectively removing any entity that was occupying that tile.
 * @param position The position of the tile to clear.
 */
void World::clear_tile(Position position) noexcept {
    if (!is_in_bounds(position)) {
        return;
    }

    overworld_occupants_[to_index(position.row)][to_index(position.col)] =
        nullptr;
}

/**
 * @brief Sets the overworld tile at the specified position to point to the
 * given entity.
 * @param position The position of the tile to set.
 * @param entity A pointer to the entity to "place" on the tile.
 */
void World::set_tile(Position position, Entity* entity) noexcept {
    if (!is_in_bounds(position)) {
        return;
    }

    overworld_occupants_[to_index(position.row)][to_index(position.col)] =
        entity;
}
