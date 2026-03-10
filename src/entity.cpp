#pragma once

#include "stats.cpp"
#include <string>

class Entity {
public:
	virtual ~Entity() = default;

    std::string name;
    Stats stats;
    int hp = stats.max_hp;

    virtual void take_dmg(int val) {
        int mitigated_damage = val - stats.defence;
        if (mitigated_damage < 0) {
            mitigated_damage = 0;
        }
        hp -= mitigated_damage;
        if (hp < 0) {
            hp = 0;
        }
    }

    virtual void heal(int val) {
        if ((hp + val) > stats.max_hp) {
            hp = stats.max_hp;
        } else if (hp <= 0) {
            return;
        } else {
            hp += val;
        }
    }

    virtual bool is_alive() const {
        return hp > 0;
    }
};
