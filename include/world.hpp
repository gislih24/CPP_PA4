#pragma once

#include "player_character.hpp"
#include "stats.hpp"
#include <string>

struct Position {
    int row{};
    int col{};
};

class World {
  public:
    // TODO: All of this is temporary until Player is implemented.
    void reset_new_game() const;
    const PlayerCharacter& get_player() const noexcept;
    int defeated_enemies() const noexcept;
    void set_player_position(Position position) noexcept;

  private:
    PlayerCharacter player_;
};