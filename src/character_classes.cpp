#include "player_character.cpp"

class Knight : public PlayerCharacter {
    void shield_brace() {
        // ability that increases defence for all allies
    }
};

class Wizard : public PlayerCharacter {
    void fireball() {
        // damage all enemies
    }
    void heal() {
        // heal target ally
    }
};
