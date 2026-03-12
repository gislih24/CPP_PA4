#include "./include/player_character.hpp"
#include "./include/entity.hpp"

PlayerCharacter::PlayerCharacter(const std::string& name, const Stats& stats,
                                 int hp)
    : Entity(name, stats, hp) {}

int PlayerCharacter::attack(Entity& target) noexcept {
    int target_hp_before = target.get_hp();
    target.take_dmg(get_stats().attack);
    int target_hp_after = target.get_hp();
    int damage_dealt = target_hp_before - target_hp_after;
    return damage_dealt;
}
