#include "./include/main_menu_state.hpp"
#include "./include/explore_state.hpp"
#include "./include/game.hpp"
#include <format>
#include <iostream>
#include <memory>

void MainMenuState::on_enter(Game&) {
    load_menu_active_ = false;
    available_saves_.clear();
    message_ = "Choose an option.";
}

void MainMenuState::render(const Game&) const {
    if (load_menu_active_) {
        std::cout << "\n"
                  << "====================\n"
                  << "      LOAD GAME     \n"
                  << "====================\n";

        if (available_saves_.empty()) {
            std::cout << "No saves available.\n";
        } else {
            for (std::size_t index = 0; index < available_saves_.size();
                 ++index) {
                std::cout << index + 1 << ") " << available_saves_[index]
                          << "\n";
            }
        }

        std::cout << "0) Back\n"
                  << "\n"
                  << message_ << "\n"
                  << "> " << std::flush;
        return;
    }

    std::cout << "\n"
              << "====================\n"
              << "      TINY JRPG     \n"
              << "====================\n"
              << "1) New Game\n"
              << "2) Load Game\n"
              << "3) Quit\n"
              << "\n"
              << message_ << "\n"
              << "> " << std::flush;
}

void MainMenuState::handle_input(Game& game, std::string_view input) {
    if (load_menu_active_) {
        handle_load_menu_input(game, input);
        return;
    }

    handle_main_menu_input(game, input);
}

void MainMenuState::refresh_load_menu(const Game& game) {
    available_saves_ = game.get_world().list_save_slots();
    if (available_saves_.empty()) {
        message_ = "No saves found. Enter '0' to return.";
        return;
    }

    message_ = "Choose a save slot to load.";
}

void MainMenuState::handle_main_menu_input(Game& game, std::string_view input) {
    const std::string choice = GameState::normalize_input(input);

    if (choice == "1" || choice == "new" || choice == "new game") {
        game.get_world().reset_new_game();
        game.request_state_change(
            std::make_unique<ExploreState>("A new adventure begins."));
        return;
    }

    if (choice == "2" || choice == "load" || choice == "load game") {
        load_menu_active_ = true;
        refresh_load_menu(game);
        return;
    }

    if (choice == "3" || choice == "quit" || choice == "exit") {
        game.quit();
        return;
    }

    message_ = "Invalid option. Enter '1', '2', or '3'.";
}

void MainMenuState::handle_load_menu_input(Game& game, std::string_view input) {
    const std::string choice = GameState::normalize_input(input);

    if (choice == "0" || choice == "back" || choice == "b") {
        load_menu_active_ = false;
        message_ = "Choose an option.";
        return;
    }

    if (available_saves_.empty()) {
        message_ = "No saves found. Enter '0' to return.";
        return;
    }

    std::string selected_slot;
    try {
        const int selected_index = std::stoi(choice);
        if (selected_index >= 1 && static_cast<std::size_t>(selected_index) <=
                                       available_saves_.size()) {
            selected_slot =
                available_saves_[static_cast<std::size_t>(selected_index) - 1];
        }
    } catch (const std::exception&) {
        for (const auto& save_name : available_saves_) {
            if (normalize_input(save_name) == choice) {
                selected_slot = save_name;
                break;
            }
        }
    }

    if (selected_slot.empty()) {
        message_ = "Invalid save selection.";
        return;
    }

    std::string error_message;
    if (!game.get_world().load_from_slot(selected_slot, error_message)) {
        message_ = error_message;
        refresh_load_menu(game);
        return;
    }

    load_menu_active_ = false;
    game.request_state_change(std::make_unique<ExploreState>(
        std::format("Loaded save '{}'.", selected_slot)));
}
