#include "./include/entity.hpp"
#include <stdint.h>
#include <string>
#include <string_view>

Entity::Entity(std::string_view name, const Stats& stats, int hp)
    : name_(name), stats_(stats), hp_(hp) {}

std::string_view Entity::get_name() const noexcept {
    return name_;
}

int Entity::get_hp() const noexcept {
    return hp_;
}

const Stats& Entity::get_stats() const noexcept {
    return stats_;
}

void Entity::take_dmg(int val) noexcept {
    int mitigated_damage = val - get_stats().defence;
    if (mitigated_damage < 0) {
        mitigated_damage = 0;
    }
    set_hp(get_hp() - mitigated_damage);
    if (get_hp() < 0) {
        set_hp(0);
    }
}

void Entity::heal(int val) noexcept {
    if ((get_hp() + val) > get_stats().max_hp) {
        set_hp(get_stats().max_hp);
    } else if (get_hp() <= 0) {
        return;
    } else {
        set_hp(get_hp() + val);
    }
}

bool Entity::is_alive() const noexcept {
    return get_hp() > 0;
}

void Entity::set_max_hp(int value) noexcept {
    stats_.max_hp = value;
}

void Entity::set_attack(int value) noexcept {
    stats_.attack = value;
}

void Entity::set_defence(int value) noexcept {
    stats_.defence = value;
}

void Entity::set_hp(int value) noexcept {
    hp_ = value;
}

int Entity::get_x_pos() const noexcept {
    return x_pos;
}

int Entity::get_y_pos() const noexcept {
    return y_pos;
}

void Entity::set_position(int new_x_pos, int new_y_pos) noexcept {
    x_pos = new_x_pos;
    y_pos = new_y_pos;
}
