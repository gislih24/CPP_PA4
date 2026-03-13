#include "./include/main_menu_state.hpp"
#include "./include/explore_state.hpp"
#include "./include/game.hpp"
#include <iostream>
#include <memory>

void MainMenuState::on_enter(Game&) {
    message_ = "Choose an option.";
}

void MainMenuState::render(const Game&) const {
    std::cout << "\n"
              << "====================\n"
              << "      TINY JRPG     \n"
              << "====================\n"
              << "1) New Game\n"
              << "2) Quit\n"
              << "\n"
              << message_ << "\n"
              << "> " << std::flush;
}

void MainMenuState::handle_input(Game& game, std::string_view input) {
    const std::string choice = GameState::normalize_input(input);

    if (choice == "1" || choice == "new" || choice == "new game") {
        game.get_world().reset_new_game();
        game.request_state_change(std::make_unique<ExploreState>());
        return;
    } else if (choice == "2" || choice == "quit" || choice == "exit") {
        game.quit();
        return;
    } else {
        message_ = "Invalid option. Enter '1' to start or '2' to quit.";
    }
}
