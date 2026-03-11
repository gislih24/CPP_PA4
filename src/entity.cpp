#include "./include/entity.hpp"
#include <string>

void Entity::take_dmg(int val) {
    int mitigated_damage = val - stats.defence;
    if (mitigated_damage < 0) {
        mitigated_damage = 0;
    }
    hp -= mitigated_damage;
    if (hp < 0) {
        hp = 0;
    }
}

void Entity::heal(int val) {
    if ((hp + val) > stats.max_hp) {
        hp = stats.max_hp;
    } else if (hp <= 0) {
        return;
    } else {
        hp += val;
    }
}

bool Entity::is_alive() const {
    return hp > 0;
}
