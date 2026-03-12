#include "./include/player_character.hpp"
#include "./include/entity.hpp"

PlayerCharacter::PlayerCharacter(std::string_view name, const Stats& stats,
                                 int hp) {
    set_max_hp(hp);
    set_hp(hp);
    set_attack(stats.attack);
    set_defence(stats.defence);
}

int PlayerCharacter::attack(Entity& target) {
    int target_hp_before = target.get_hp();
    target.take_dmg(get_stats().attack);
    int target_hp_after = target.get_hp();
    int damage_dealt = target_hp_before - target_hp_after;
    return damage_dealt;
}
