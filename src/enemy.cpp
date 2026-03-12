#include "./include/enemy.hpp"

int Enemy::attack(Entity& target) {
    target_hp_before = target.get_hp();
    target.take_dmg(get_stats().attack);
    target_hp_after = target.get_hp();
    damage_dealt = target_hp_before - target_hp_after;
    return damage_dealt;
}