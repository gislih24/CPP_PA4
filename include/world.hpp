#pragma once

#include "player_character.hpp"
#include <string>

struct Position {
    int row{};
    int col{};
};

class World {
  public:
    // TODO: All of this is temporary until Player is implemented.
    void reset_new_game() {
        player_name_ = "Hero";
        player_max_hp_ = 30;
        player_hp_ = player_max_hp_;
        player_position_ = {1, 1};
        defeated_enemies_ = 0;
    }

    const std::string& player_name() const noexcept {
        return player_name_;
    }

    int player_hp() const noexcept {
        return player_hp_;
    }

    int player_max_hp() const noexcept {
        return player_max_hp_;
    }

    Position player_position() const noexcept {
        return player_position_;
    }

    int defeated_enemies() const noexcept {
        return defeated_enemies_;
    }

    void set_player_position(Position position) noexcept {
        player_position_ = position;
    }

  private:
    std::string player_name_{"Hero"};
    int player_max_hp_{30};
    int player_hp_{30};
    Position player_position_{1, 1};
    int defeated_enemies_{0};
};