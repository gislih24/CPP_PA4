#include "./include/entity.hpp"
#include <string>
#include <string_view>

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
    int mitigated_damage = val - get_stats().defence;
    if (mitigated_damage < 0) {
        mitigated_damage = 0;
    }
    set_hp(get_hp() - mitigated_damage);
    if (get_hp() < 0) {
        set_hp(0);
    }
}

void Entity::heal(int val) {
    if ((get_hp() + val) > get_stats().max_hp) {
        set_hp(get_stats().max_hp);
    } else if (get_hp() <= 0) {
        return;
    } else {
        set_hp(get_hp() + val);
    }
}

bool Entity::is_alive() const {
    return get_hp() > 0;
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