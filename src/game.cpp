#include "./include/game.hpp"
#include "./include/game_state.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

Game::Game(std::unique_ptr<GameState> initial_state)
    : current_state_(std::move(initial_state)) {
    if (!current_state_) {
        throw std::invalid_argument("Game requires a non-null initial state.");
    }

    current_state_->on_enter(*this);
    apply_pending_state_change();
}

/**
 * @brief The main game loop.
 *
 * Continues until the user quits the game. Each iteration renders the current
 * state, gets input from the user, and then handles that input.
 */
void Game::run() {
    while (game_is_running_) {
        // Apply any pending state changes before rendering and handling input.
        apply_pending_state_change();

        if (!game_is_running_) {
            continue;
        }

        // Let the current state render itself, with this game as context.
        current_state_->render(*this);

        // Try to get the input. If we fail, we quit.
        std::string input;
        if (!std::getline(std::cin, input)) {
            quit();
            continue;
        }
        // Let the current state handle the input.
        current_state_->handle_input(*this, input);
        // Apply any pending state changes *after* handling input, in case the
        // input caused a state change.
        apply_pending_state_change();
    }

    std::cout << "\n*Mario voice*: Thank you-a so much for playing my game.\n";
}

/**
 * @brief A method that lets game states request a new state.
 *
 * E.g., the MainMenuState might request to go to the ExploreState after a
 * player selects a certain option.
 * @param next_state The next state to transition to.
 */
void Game::request_state_change(std::unique_ptr<GameState> next_state) {
    pending_state_ = std::move(next_state);
}

// Quit the game.
void Game::quit() noexcept {
    game_is_running_ = false;
}

bool Game::is_running() const noexcept {
    return game_is_running_;
}

World& Game::get_world() noexcept {
    return world_;
}

const World& Game::get_world() const noexcept {
    return world_;
}

/**
 * @brief Applies pending state changes until there are no more pending states.
 *
 * This allows for multiple state changes to be requested in a single frame,
 * and ensures that the game is always in the most up-to-date state.
 */
void Game::apply_pending_state_change() {
    while (game_is_running_ && pending_state_) {
        current_state_ = std::move(pending_state_);
        current_state_->on_enter(*this);
    }
}
