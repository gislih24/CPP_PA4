#include "player_character.hpp"

int PlayerCharacter::attack(Entity& target) {
    target_hp_before = target.hp;
    target.take_dmg(stats.attack);
    target_hp_after = target.hp;
    damage_dealt = target_hp_before - target_hp_after;
    return damage_dealt;
}
