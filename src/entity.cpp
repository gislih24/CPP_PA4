#include "entity.hpp"
#include "stats.hpp"
#include <string>

class Entity {
  public:
    virtual ~Entity() = default;

    std::string name;
    Stats stats;
    int hp;

    void take_dmg(int val) {
        hp -= val;
    }

    void heal(int val) {
        if ((hp + val) > stats.max_hp) {
            hp = stats.max_hp;
        } else if (hp >= 0) {
            return;
        } else {
            hp += val;
        }
    }

    bool is_alive() {
        if (hp <= 0) {
            return false;
        } else {
            return true;
        }
    }
};
