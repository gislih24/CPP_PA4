#pragma once

#include "player_character.hpp"

class Knight : public PlayerCharacter {
	public:
		void shield_brace();
};

class Wizard : public PlayerCharacter {
	public:
		void fireball();
		void heal();
};
