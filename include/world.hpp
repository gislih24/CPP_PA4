#pragma once

#include "enemy.hpp"
#include "entity.hpp"
#include "player_character.hpp"
#include <memory>
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
    PlayerCharacter& get_player() noexcept;
    const PlayerCharacter& get_player() const noexcept;
    const std::vector<std::unique_ptr<Enemy>>& get_enemies() const noexcept;
    const std::vector<std::vector<Entity*>>&
    get_overworld_occupants() const noexcept;
    int defeated_enemies() const noexcept;
    MoveOutcome try_move_player(int row_change, int col_change) noexcept;
    void set_player_position(Position position) noexcept;
    bool remove_enemy(Enemy const* enemy) noexcept;

    //   private:
    PlayerCharacter player_;
    std::vector<std::unique_ptr<Enemy>> enemies_;
    std::vector<std::vector<Entity*>> overworld_occupants_;
    int defeated_enemies_{};

    void populate_overworld();
    void move_entity(Entity* entity, int new_x_pos, int new_y_pos) noexcept;
    bool is_in_bounds(Position position) const noexcept;
    Entity* get_occupant_at(Position position) const noexcept;
    void clear_tile(Position position) noexcept;
    void set_tile(Position position, Entity* entity) noexcept;
};
