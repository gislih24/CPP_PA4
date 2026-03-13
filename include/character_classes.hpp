#pragma once

#include "player_character.hpp"

class Knight : public PlayerCharacter {
  public:
    Knight();
    Knight(const Stats& stats, int hp);
    void shield_brace() noexcept;
};

class Wizard : public PlayerCharacter {
  public:
    Wizard();
    int fireball(Entity& target) const noexcept;
    void healing_touch(PlayerCharacter& target) const noexcept;
};
