#include "game_state.hpp"
#include "main_menu_state.hpp"
#include <memory>

class Game {
  private:
    std::unique_ptr<GameState> state;

  public:
    Game();
    ~Game();
    void run();
};

Game::Game() {
    // Initialize the game state here, e.g., state =
    // std::make_unique<MainMenuState>();
    state = std::make_unique<MainMenuState>();
}