#include "./include/player_character.hpp"
#include "./include/entity.hpp"

int PlayerCharacter::attack(Entity& target) {
    const int target_hp_before = target.get_hp();
    target.take_dmg(get_stats().attack);
    const int target_hp_after = target.get_hp();
    const int damage_dealt = target_hp_before - target_hp_after;
    return damage_dealt;
}
