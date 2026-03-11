#include "entity.cpp"

class Enemy : public Entity {
  public:
    int target_hp_before;
    int target_hp_after;
    int damage_dealt;

    virtual int attack(Entity& target) {
        target_hp_before = target.hp;
        target.take_dmg(stats.attack);
        target_hp_after = target.hp;
        damage_dealt = target_hp_before - target_hp_after;
        return (damage_dealt);
    }
};