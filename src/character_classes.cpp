#include "./include/character_classes.hpp"

Knight::Knight() : PlayerCharacter("Knight", Stats{14, 3, 2}, 14) {}

Knight::Knight(const Stats& stats, int hp)
    : PlayerCharacter("Knight", stats, hp) {}

void Knight::shield_brace() noexcept {
    // Buff logic is handled in BattleState.
}

Wizard::Wizard() : PlayerCharacter("Wizard", Stats{8, 5, 0}, 8) {}

int Wizard::fireball(Entity& target) const noexcept {
    const int damage = get_stats().attack * 2;
    target.take_dmg(damage);
    return damage;
}

void Wizard::healing_touch(PlayerCharacter& target) const noexcept {
    target.heal(3);
}
