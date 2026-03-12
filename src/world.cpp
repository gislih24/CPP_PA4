#include "world.hpp"

void World::reset_new_game() const {
    auto initial_stats = Stats{8, 10, 12};
    auto player_ =
        PlayerCharacter{"The Knight", initial_stats, initial_stats.max_hp};
}

const PlayerCharacter& World::get_player() const noexcept {
    return player_;
}