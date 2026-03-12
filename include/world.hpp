#pragma once

#include "enemy.hpp"
#include "player_character.hpp"
#include "stats.hpp"
#include <string>
#include <vector>

struct Position {
    int row{};
    int col{};
};

class World {
  public:
    void reset_new_game() const;
    const PlayerCharacter& get_player() const noexcept;
    int defeated_enemies() const noexcept;
    void set_player_position(Position position) noexcept;

  private:
    PlayerCharacter player_;
    std::vector<Enemy> enemies_;
};