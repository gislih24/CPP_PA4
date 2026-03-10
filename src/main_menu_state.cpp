#include "main_menu_state.hpp"
#include "game_state.hpp"

enum command_options { START_GAME = 0, QUIT_GAME = 1 };

class MainMenuState : public GameState {
  public:
    MainMenuState();
    ~MainMenuState();
    void render() override;
    void handle_input() override;
    void update() override;
};

MainMenuState::MainMenuState() {}

MainMenuState::~MainMenuState() {}

void MainMenuState::render() {
    // Render the main menu here
}

void MainMenuState::handle_input() {
    // Handle user input for the main menu here
}

void MainMenuState::update() {
    // Update the main menu state here
}