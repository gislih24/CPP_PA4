#include "./include/explore_state.hpp"
#include <print>

void ExploreState::on_enter(Game& game) override {
    combat_log_.emplace_back(
        "Against all the evil that Hell can conjure, all the wickedness\n ");
    combat_log_.emplace_back(
        "that mankind can produce, we will send unto them... only you.\n");
    combat_log_.emplace_back("Rip and tear, until it is done.\n");
}

void ExploreState::render(const Game&) const {
    // For each of the message vectors
    for (const auto* message_vector :
         {&combat_log_, &status_display_, &action_menu_}) {
        // For each message in the current message vector
        for (const auto& message : *message_vector) {
            std::print("{}", message); // Print it
        }
    }
}

void ExploreState::handle_input(Game& game, std::string_view input) override {}
