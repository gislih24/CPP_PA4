#pragma once

#include "stats.hpp"
#include <string>
#include <string_view>

class Entity {
  public:
    std::string_view get_name() const;
    int get_hp() const;
    const Stats& get_stats() const;

    virtual void take_dmg(int val);
    virtual void heal(int val);
    virtual bool is_alive() const;

    virtual ~Entity() = default;

    void set_max_hp(int value);
    void set_attack(int value);
    void set_defence(int value);
    void set_hp(int value);

  private:
    std::string name_;
    Stats stats_;
    int hp_;
};