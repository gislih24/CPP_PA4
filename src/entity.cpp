#include "./include/entity.hpp"
#include <string>

Entity::Entity(std::string_view name, const Stats& stats, int hp)
    : name_(name), stats_(stats), hp_(hp) {}

std::string_view Entity::get_name() const {
    return name_;
}

int Entity::get_hp() const {
    return hp_;
}

const Stats& Entity::get_stats() const {
    return stats_;
}

void Entity::take_dmg(int val) {
    int mitigated_damage = val - stats_.defence;
    if (mitigated_damage < 0) {
        mitigated_damage = 0;
    }
    hp_ -= mitigated_damage;
    if (hp_ < 0) {
        hp_ = 0;
    }
}

void Entity::heal(int val) {
    if ((hp_ + val) > stats_.max_hp) {
        hp_ = stats_.max_hp;
    } else if (hp_ <= 0) {
        return;
    } else {
        hp_ += val;
    }
}

bool Entity::is_alive() const {
    return hp_ > 0;
}

void Entity::set_max_hp(int value) {
    stats_.max_hp = value;
}

void Entity::set_attack(int value) {
    stats_.attack = value;
}

void Entity::set_defence(int value) {
    stats_.defence = value;
}

void Entity::set_hp(int value) {
    hp_ = value;
}
