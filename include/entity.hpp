#pragma once

#include "stats.hpp"
#include <string>
#include <string_view>
#include <utility>

class Entity {
  public:
    Entity(std::string_view name, const Stats& stats, int hp);
    Entity() = default;

    std::string_view get_name() const noexcept;
    int get_hp() const noexcept;
    const Stats& get_stats() const noexcept;

    virtual void take_dmg(int val) noexcept;
    virtual void heal(int val) noexcept;
    virtual bool is_alive() const noexcept;
    int get_x_pos() const noexcept;
    int get_y_pos() const noexcept;

    virtual ~Entity() = default;

    void set_max_hp(int value) noexcept;
    void set_attack(int value) noexcept;
    void set_defence(int value) noexcept;
    void set_hp(int value) noexcept;
    void set_position(int new_x_pos, int new_y_pos) noexcept;

  private:
    std::string name_{};
    Stats stats_{};
    int hp_{};
    int x_pos{};
    int y_pos{};
};
