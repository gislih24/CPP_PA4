#include "./include/character_classes.hpp"

Knight::Knight() : PlayerCharacter("Knight", Stats{14, 3, 2}, 14) {}

Knight::Knight(const Stats& stats, int hp)
    : PlayerCharacter("Knight", stats, hp) {}

void Knight::shield_brace() noexcept {
    // Buff logic is handled in BattleState.
}

Wizard::Wizard() : PlayerCharacter("Wizard", Stats{8, 5, 0}, 8) {}

int Wizard::fireball(Entity& target) noexcept {
    const int target_hp_before = target.get_hp();
    const int raw_damage = get_stats().attack * 2;
    target.take_dmg(raw_damage);
    const int target_hp_after = target.get_hp();
    return target_hp_before - target_hp_after;
}

void Wizard::healing_touch(PlayerCharacter& target) const noexcept {
    target.heal(3);
}

int Wizard::fireball_cooldown() const noexcept {
    return fireball_cooldown_;
}

void Wizard::set_fireball_cooldown(int value) noexcept {
    fireball_cooldown_ = value < 0 ? 0 : value;
}

void Wizard::decrement_fireball_cooldown() noexcept {
    if (fireball_cooldown_ > 0) {
        --fireball_cooldown_;
    }
}
