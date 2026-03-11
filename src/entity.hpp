#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "stats.hpp"
#include <string>

class Entity {
  public:
    virtual ~Entity() = default;

    std::string name;
    Stats stats;
    int hp;

    virtual void take_dmg(int val);
    virtual void heal(int val);
    virtual bool is_alive() const;
};

#endif // ENTITY_HPP