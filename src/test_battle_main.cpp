#include "./include/game.hpp"
#include "./include/battle_state.hpp"
#include <memory>

int main() {
    // Start the game directly in a BattleState so we can test the
    // combat loop in isolation.
    auto initial_state = std::make_unique<BattleState>();
    Game game{std::move(initial_state)};
    game.run();
    return 0;
}
