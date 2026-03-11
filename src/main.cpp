#include "game.hpp"
#include "main_menu_state.hpp"
#include <memory>

int main() {
    Game game(std::make_unique<MainMenuState>());
    game.run();
    return 0;
}