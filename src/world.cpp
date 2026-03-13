#include "./include/world.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
#include <random>

namespace {
constexpr int WORLD_HEIGHT = 5;
constexpr int WORLD_WIDTH = 5;
constexpr std::string_view TINY_JRPG_SAVE_HEADER = "TINY_JRPG_SAVE_V1";
constexpr std::string_view SAVE_DIRECTORY = "saves";
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
    current_save_slot_.clear();
    defeated_enemies_ = 0;
    enemies_.clear();
    for (auto& row : overworld_occupants_) {
        std::ranges::fill(row, nullptr);
    }

    auto initial_stats = Stats{15, 6, 2};
    player_ =
        PlayerCharacter{"The Knight", initial_stats, initial_stats.max_hp};
    player_.set_position(0, 0);

    std::mt19937 rng(std::random_device{}());            // create once
    std::uniform_int_distribution<int> dist_count(3, 5); // how many enemies

    int x = dist_count(rng);                             // 3–5
    int silly_slime_count = 0;
    int evil_skeleton_count = 0;
    int three_gnomes_in_a_trenchcoat_count = 0;

    std::uniform_int_distribution<int> dist_kind(1, 3);  // which kind of enemy
    std::uniform_int_distribution<int> dist_row(0, WORLD_HEIGHT - 1);
    std::uniform_int_distribution<int> dist_col(0, WORLD_WIDTH - 1);

    // Track occupied tiles so each enemy gets a unique position and never
    // starts on the player's tile.
    std::vector<Position> occupied_tiles;
    occupied_tiles.push_back(
        Position{player_.get_x_pos(), player_.get_y_pos()});

    for (int i = 0; i < x; ++i) {
        int kind = dist_kind(rng);

        // Find a free random tile.
        int row = 0;
        int col = 0;
        while (true) {
            row = dist_row(rng);
            col = dist_col(rng);

            bool taken = false;
            for (const auto& pos : occupied_tiles) {
                if (pos.row == row && pos.col == col) {
                    taken = true;
                    break;
                }
            }
            if (!taken) {
                break;
            }
        }
        if (kind == 1) {
            std::string new_enemy_name = std::format("Silly Slime {}", silly_slime_count);
            ++silly_slime_count;
            auto new_enemy_stats = Stats{40 + i, 3 + i, 1 + i}; // index added to stats for variability
            auto new_enemy = std::make_unique<Enemy>(
                new_enemy_name, new_enemy_stats, new_enemy_stats.max_hp);
            new_enemy->set_position(row, col);
            enemies_.push_back(std::move(new_enemy));
        } else if (kind == 2) {
            std::string new_enemy_name = std::format("Evil Skeleton {}", evil_skeleton_count);
            ++evil_skeleton_count;
            auto new_enemy_stats = Stats{20 + i, 4 + i, 0 + i}; // index added to stats for variability
            auto new_enemy = std::make_unique<Enemy>(
                new_enemy_name, new_enemy_stats, new_enemy_stats.max_hp);
            new_enemy->set_position(row, col);
            enemies_.push_back(std::move(new_enemy));
        } else { // kind == 3
            std::string new_enemy_name = std::format("Three Gnomes in a Trenchcoat {}", three_gnomes_in_a_trenchcoat_count);
            ++three_gnomes_in_a_trenchcoat_count;
            auto new_enemy_stats = Stats{30 + i, 3 + i, 2 + i}; // index added to stats for variability
            auto new_enemy = std::make_unique<Enemy>(
                new_enemy_name, new_enemy_stats, new_enemy_stats.max_hp);
            new_enemy->set_position(row, col);
            enemies_.push_back(std::move(new_enemy));
        }

        occupied_tiles.push_back(Position{row, col});
    }

    populate_overworld();
}

/**
 * @brief Saves the current state of the world to a save slot with the
 * specified name.
 * @param slot_name The name of the save slot to save to.
 * @param error_message An output parameter that will contain an error message
 * if the save fails.
 * @return True if the save was successful, false otherwise.
 */
bool World::save_to_slot(std::string_view slot_name,
                         std::string& error_message) {
    error_message.clear();

    const std::string trimmed_slot_name = trim_copy(slot_name);
    if (trimmed_slot_name.empty() ||
        sanitize_slot_name(trimmed_slot_name).empty()) {
        error_message = "Save name must contain letters or numbers.";
        return false;
    }

    std::error_code filesystem_error;
    std::filesystem::create_directories(get_save_directory(), filesystem_error);
    if (filesystem_error) {
        error_message = "Failed to create the save directory.";
        return false;
    }

    std::ofstream save_file(build_slot_save_path(trimmed_slot_name));
    if (!save_file) {
        error_message = "Failed to open the save file for writing.";
        return false;
    }

    save_file << TINY_JRPG_SAVE_HEADER << '\n';
    save_file << "slot_name " << std::quoted(trimmed_slot_name) << '\n';
    save_file << "defeated_enemies " << defeated_enemies_ << '\n';
    save_file << "player " << std::quoted(std::string(player_.get_name()))
              << ' ' << player_.get_hp() << ' ' << player_.get_stats().max_hp
              << ' ' << player_.get_stats().attack << ' '
              << player_.get_stats().defence << ' ' << player_.get_x_pos()
              << ' ' << player_.get_y_pos() << '\n';
    save_file << "enemy_count " << enemies_.size() << '\n';

    for (const auto& enemy : enemies_) {
        if (enemy == nullptr) {
            continue;
        }

        save_file << "enemy " << std::quoted(std::string(enemy->get_name()))
                  << ' ' << enemy->get_hp() << ' ' << enemy->get_stats().max_hp
                  << ' ' << enemy->get_stats().attack << ' '
                  << enemy->get_stats().defence << ' ' << enemy->get_x_pos()
                  << ' ' << enemy->get_y_pos() << '\n';
    }

    if (!save_file.good()) {
        error_message = "Failed while writing the save file.";
        return false;
    }

    current_save_slot_ = trimmed_slot_name;
    return true;
}

/**
 * @brief Loads the world state/data from a save slot with the specified name.
 * @param slot_name The name of the save slot to load from.
 * @param error_message An output parameter that will contain an error message
 * if the load fails.
 * @return True if the load was successful, false otherwise.
 */
bool World::load_from_slot(std::string_view slot_name,
                           std::string& error_message) {
    error_message.clear();

    const std::string trimmed_slot_name = trim_copy(slot_name);
    if (trimmed_slot_name.empty() ||
        sanitize_slot_name(trimmed_slot_name).empty()) {
        error_message = "Invalid save name.";
        return false;
    }

    std::ifstream save_file(build_slot_save_path(trimmed_slot_name));
    if (!save_file) {
        error_message = "Save file not found.";
        return false;
    }

    std::string save_header_line;
    std::getline(save_file, save_header_line);
    if (save_header_line != TINY_JRPG_SAVE_HEADER) {
        error_message = "Save file format is invalid.";
        return false;
    }

    std::string loaded_slot_name;
    int loaded_defeated_enemies = 0;
    std::string player_name;
    int player_hp = 0;
    int player_max_hp = 0;
    int player_attack = 0;
    int player_defence = 0;
    int player_row = 0;
    int player_col = 0;
    std::size_t enemy_count = 0;

    if (!read_labeled_value(save_file, "slot_name") ||
        !(save_file >> std::quoted(loaded_slot_name)) ||
        !read_labeled_value(save_file, "defeated_enemies") ||
        !(save_file >> loaded_defeated_enemies) ||
        !read_labeled_value(save_file, "player") ||
        !(save_file >> std::quoted(player_name) >> player_hp >> player_max_hp >>
          player_attack >> player_defence >> player_row >> player_col) ||
        !read_labeled_value(save_file, "enemy_count") ||
        !(save_file >> enemy_count)) {
        error_message = "Save file contents are incomplete.";
        return false;
    }

    if (loaded_defeated_enemies < 0 || player_max_hp < 0 || player_attack < 0 ||
        player_defence < 0 || player_hp < 0 || player_hp > player_max_hp ||
        !is_in_bounds({player_row, player_col})) {
        error_message = "Save file contains invalid player data.";
        return false;
    }

    std::vector<std::vector<bool>> is_tile_occupied(
        WORLD_HEIGHT, std::vector<bool>(WORLD_WIDTH, false));
    is_tile_occupied[to_index(player_row)][to_index(player_col)] = true;

    PlayerCharacter loaded_player{
        player_name, Stats{player_max_hp, player_attack, player_defence},
        player_hp};
    loaded_player.set_position(player_row, player_col);

    std::vector<std::unique_ptr<Enemy>> loaded_enemies;
    loaded_enemies.reserve(enemy_count);

    for (std::size_t index = 0; index < enemy_count; ++index) {
        std::string enemy_name;
        int enemy_hp = 0;
        int enemy_max_hp = 0;
        int enemy_attack = 0;
        int enemy_defence = 0;
        int enemy_row = 0;
        int enemy_col = 0;

        if (!read_labeled_value(save_file, "enemy") ||
            !(save_file >> std::quoted(enemy_name) >> enemy_hp >>
              enemy_max_hp >> enemy_attack >> enemy_defence >> enemy_row >>
              enemy_col)) {
            error_message = "Save file contains incomplete enemy data.";
            return false;
        }

        if (enemy_max_hp < 0 || enemy_attack < 0 || enemy_defence < 0 ||
            enemy_hp < 0 || enemy_hp > enemy_max_hp ||
            !is_in_bounds({enemy_row, enemy_col}) ||
            is_tile_occupied[to_index(enemy_row)][to_index(enemy_col)]) {
            error_message = "Save file contains invalid enemy data.";
            return false;
        }

        is_tile_occupied[to_index(enemy_row)][to_index(enemy_col)] = true;

        auto loaded_enemy = std::make_unique<Enemy>(
            enemy_name, Stats{enemy_max_hp, enemy_attack, enemy_defence},
            enemy_hp);
        loaded_enemy->set_position(enemy_row, enemy_col);
        loaded_enemies.push_back(std::move(loaded_enemy));
    }

    for (auto& row : overworld_occupants_) {
        std::ranges::fill(row, nullptr);
    }

    player_ = std::move(loaded_player);
    enemies_ = std::move(loaded_enemies);
    defeated_enemies_ = loaded_defeated_enemies;
    current_save_slot_ =
        loaded_slot_name.empty() ? trimmed_slot_name : loaded_slot_name;
    populate_overworld();
    return true;
}

/**
 * @brief Retrieves a list of available save slots by scanning the save
 * directory for valid save files and gets their slot names.
 * @return A vector of strings representing the names of available save slots.
 */
std::vector<std::string> World::list_save_slots() const {
    std::vector<std::string> save_slots;

    std::error_code filesystem_error;
    if (!std::filesystem::exists(get_save_directory(), filesystem_error) ||
        filesystem_error) {
        return save_slots;
    }

    for (const auto& entry : std::filesystem::directory_iterator(
             get_save_directory(), filesystem_error)) {
        if (filesystem_error || !entry.is_regular_file() ||
            entry.path().extension() != ".txt") {
            continue;
        }

        std::ifstream save_file(entry.path());
        if (!save_file) {
            continue;
        }

        std::string magic_line;
        std::getline(save_file, magic_line);
        if (magic_line != TINY_JRPG_SAVE_HEADER) {
            continue;
        }

        std::string slot_name;
        if (!read_labeled_value(save_file, "slot_name") ||
            !(save_file >> std::quoted(slot_name)) || slot_name.empty()) {
            slot_name = entry.path().stem().string();
        }

        save_slots.push_back(slot_name);
    }

    std::ranges::sort(save_slots);
    return save_slots;
}

/* ---------------------------------Getters--------------------------------- */
std::string_view World::get_current_save_slot() const noexcept {
    return current_save_slot_;
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

int World::get_defeated_enemies() const noexcept {
    return defeated_enemies_;
}

/**
 * @brief Attempts to move the player by the specified row and column changes.
 * @param row_change The change in the player's row position (positive for
 * down, negative for up).
 * @param col_change The change in the player's column position (positive for
 * right, negative for left).
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

    const Position enemy_position{enemy->get_x_pos(), enemy->get_y_pos()};
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

    if (is_in_bounds(enemy_position) &&
        get_occupant_at(enemy_position) == enemy) {
        clear_tile(enemy_position);
    }

    defeated_enemies_ += removed;
    return true;
}

/**
 * @brief Populates the overworld grid with the player and enemies based on
 * their current positions.
 */
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

std::size_t World::to_index(int value) const {
    return static_cast<std::size_t>(value);
}

/**
 * @brief Creates and returns a trimmed copy of the input string, removing
 * leading and trailing whitespace characters.
 * @param input The string to be trimmed.
 * @return A new string with leading and trailing whitespace removed.
 */
std::string World::trim_copy(std::string_view input) const {
    std::size_t start = 0;
    while (start < input.size() &&
           std::isspace(static_cast<unsigned char>(input[start]))) {
        ++start;
    }

    std::size_t end = input.size();
    while (end > start &&
           std::isspace(static_cast<unsigned char>(input[end - 1]))) {
        --end;
    }

    return std::string(input.substr(start, end - start));
}

/**
 * @brief Sanitizes the provided slot name by removing invalid characters and
 * replacing sequences of spaces, dashes, or underscores with a single
 * underscore.
 * @param slot_name The slot name to sanitize.
 * @return The sanitized slot name.
 */
std::string World::sanitize_slot_name(std::string_view slot_name) const {
    std::string sanitized_slot_name;

    for (char curr_char : slot_name) {
        if (const auto curr_char_unsigned =
                static_cast<unsigned char>(curr_char);
            std::isalnum(curr_char_unsigned)) {
            sanitized_slot_name.push_back(
                static_cast<char>(std::tolower(curr_char_unsigned)));
            continue;
        }

        if ((curr_char == ' ' || curr_char == '-' || curr_char == '_') &&
            (sanitized_slot_name.empty() ||
             sanitized_slot_name.back() != '_')) {
            sanitized_slot_name.push_back('_');
        }
    }

    while (!sanitized_slot_name.empty() && sanitized_slot_name.back() == '_') {
        sanitized_slot_name.pop_back();
    }

    return sanitized_slot_name;
}

/**
 * @brief Gets the path to the save directory where game saves are stored. If
 * the directory does not exist, it will be created when saving a game.
 * @return The path to the save directory as a std::filesystem::path object.
 */
std::filesystem::path World::get_save_directory() const {
    return std::filesystem::path{SAVE_DIRECTORY};
}

/**
 * @brief Builds the full file path for a save slot based on the provided slot
 * name, by sanitizing the slot name and appending the appropriate file
 * extension in the save directory.
 * @param slot_name The name of the save slot to build the file path for.
 * @return The full file path for the save slot as a std::filesystem::path
 * object.
 */
std::filesystem::path
World::build_slot_save_path(std::string_view slot_name) const {
    return get_save_directory() /
           std::format("{}.txt", sanitize_slot_name(slot_name));
}

/**
 * @brief Reads a labeled value from the input stream, expecting a specific
 * label before the value.
 *
 * This is used to ensure that the save file has the correct format and that
 * the expected data is present.
 * @param input The input file stream to read from.
 * @param expected_label The label expected before the value.
 * @return True if the expected label is found and the value is read
 * successfully; false otherwise.
 */
bool World::read_labeled_value(std::istream& input,
                               std::string_view expected_label) const {
    std::string label;
    return static_cast<bool>(input >> label) && label == expected_label;
}