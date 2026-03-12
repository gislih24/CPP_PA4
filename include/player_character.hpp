#pragma once

#include "entity.hpp"

class PlayerCharacter : public Entity {
  public:
    PlayerCharacter(std::string_view name, const Stats& stats, int hp);
    virtual int attack(Entity& target);
};
