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
    int fireball(Entity& target) noexcept;
    void healing_touch(PlayerCharacter& target) const noexcept;

    int fireball_cooldown() const noexcept;
    void set_fireball_cooldown(int value) noexcept;
    void decrement_fireball_cooldown() noexcept;

  private:
    int fireball_cooldown_ = 0;
};