#pragma once

#include "enemy.hpp"
#include "entity.hpp"
#include "player_character.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

struct Position {
    int row{};
    int col{};
};

enum class MoveResult { moved, enemy_encounter, blocked, out_of_bounds };

struct MoveOutcome {
    MoveResult result{MoveResult::blocked};
    Position destination{};
    Enemy* enemy{nullptr};
};

class World {
  public:
    World();
    void reset_new_game();
    bool save_to_slot(std::string_view slot_name, std::string& error_message);
    bool load_from_slot(std::string_view slot_name, std::string& error_message);
    std::vector<std::string> list_save_slots() const;
    std::string_view get_current_save_slot() const noexcept;
    PlayerCharacter& get_player() noexcept;
    const PlayerCharacter& get_player() const noexcept;
    const std::vector<std::unique_ptr<Enemy>>& get_enemies() const noexcept;
    const std::vector<std::vector<Entity*>>&
    get_overworld_occupants() const noexcept;
    int get_defeated_enemies() const noexcept;
    MoveOutcome try_move_player(int row_change, int col_change) noexcept;
    void set_player_position(Position position) noexcept;
    bool remove_enemy(Enemy const* enemy) noexcept;

  private:
    PlayerCharacter player_;
    std::vector<std::unique_ptr<Enemy>> enemies_;
    std::vector<std::vector<Entity*>> overworld_occupants_;
    int defeated_enemies_{};
    std::string current_save_slot_{};

    void populate_overworld();
    void move_entity(Entity* entity, int new_x_pos, int new_y_pos) noexcept;
    bool is_in_bounds(Position position) const noexcept;
    Entity* get_occupant_at(Position position) const noexcept;
    void clear_tile(Position position) noexcept;
    void set_tile(Position position, Entity* entity) noexcept;
    std::size_t to_index(int value) const;
    std::string trim_copy(std::string_view input) const;
    std::string sanitize_slot_name(std::string_view slot_name) const;
    std::filesystem::path get_save_directory() const;
    std::filesystem::path
    build_slot_save_path(std::string_view slot_name) const;
    bool read_labeled_value(std::istream& input,
                            std::string_view expected_label) const;
};
