#pragma once

#include "enemy.hpp"
#include "entity.hpp"
#include "player_character.hpp"
#include "stats.hpp"
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

struct Position {
    int row{};
    int col{};
};

class World {
  public:
    World();
    void reset_new_game();
    const PlayerCharacter& get_player() const noexcept;
    const std::vector<std::unique_ptr<Enemy>>& get_enemies() const noexcept;
    int defeated_enemies() const noexcept;
    void set_player_position(Position position) noexcept;

    //   private:
    PlayerCharacter player_;
    std::vector<std::unique_ptr<Enemy>> enemies_;
    std::vector<std::vector<Entity*>> overworld_occupants_;

    void populate_overworld();
    void move_entity(Entity* entity, int_fast8_t new_x_pos,
                     int_fast8_t new_y_pos) noexcept;
};
