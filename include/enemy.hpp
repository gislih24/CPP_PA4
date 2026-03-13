#pragma once

#include "entity.hpp"

class Enemy : public Entity {
  public:
    int target_hp_before = 0;
    int target_hp_after = 0;
    int damage_dealt = 0;

    Enemy(const std::string& name, const Stats& stats, int hp);
    virtual int attack(Entity& target) noexcept;
};
