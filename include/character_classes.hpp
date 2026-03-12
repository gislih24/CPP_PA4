#pragma once

#include "player_character.hpp"

class Knight : public PlayerCharacter {
	public:
		void shield_brace();
};

class Wizard : public PlayerCharacter {
	public:
		int fireball(Entity& target);
		// Heals the chosen party member
		void healing_touch(PlayerCharacter& target);
};
