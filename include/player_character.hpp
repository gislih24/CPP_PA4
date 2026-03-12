#pragma once

#include "entity.hpp"

class PlayerCharacter : public Entity {
  public:
    virtual int attack(Entity& target);
};
