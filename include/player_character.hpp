#pragma once

#include "entity.hpp"

class PlayerCharacter : public Entity {
  public:
    PlayerCharacter(const std::string& name, const Stats& stats, int hp);
    virtual int attack(Entity& target) noexcept;
};
