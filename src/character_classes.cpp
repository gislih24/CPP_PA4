#include "./include/character_classes.hpp"

void Knight::shield_brace() {
	// Buff logic is handled in BattleState; this can later track Knight-
	// specific state if needed.
}

int Wizard::fireball(Entity& target) {
	const int damage = (get_stats().attack * 2);
	target.take_dmg(damage);
	return damage;
}

void Wizard::healing_touch(PlayerCharacter& target) {
	target.heal(3);
}
