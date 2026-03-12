#pragma once

#include "stats.hpp"
#include <stdint.h>
#include <string>
#include <string_view>
#include <utility>

class Entity {
  public:
    Entity(std::string_view name, const Stats& stats, int hp);
    Entity() = default;

    std::string_view get_name() const;
    int get_hp() const;
    const Stats& get_stats() const;

    virtual void take_dmg(int val);
    virtual void heal(int val);
    virtual bool is_alive() const;
    int_fast8_t Entity::get_x_pos() const;
    int_fast8_t Entity::get_y_pos() const;

    virtual ~Entity() = default;

    void set_max_hp(int value);
    void set_attack(int value);
    void set_defence(int value);
    void set_hp(int value);
    void set_position(int_fast8_t new_x_pos, int_fast8_t new_y_pos);

  private:
    std::string name_;
    Stats stats_;
    int hp_;
    int_fast8_t x_pos;
    int_fast8_t y_pos;
};