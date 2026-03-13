#include "./include/explore_state.hpp"
#include "./include/battle_state.hpp"
#include "./include/game.hpp"
#include <memory>
#include <print>
#include <utility>

ExploreState::ExploreState(std::string status_message)
    : status_message_(std::move(status_message)) {}

void ExploreState::on_enter(Game& game) {
    if (status_message_.empty()) {
        if (game.get_world().get_enemies().empty()) {
            status_message_ = "The overworld is clear. No enemies remain... I"
                              "hope you're happy.";
        } else {
            status_message_ = "Move with W/A/S/D. Step onto '#' to face a foe "
                              "and start a battle!";
        }
    }

    rebuild_ui(game);
}

void ExploreState::render(const Game& game) const {
    build_map(game);
    // For each of the message vectors
    for (const auto* message_vector :
         {&overworld_map_, &status_display_, &action_menu_}) {
        // For each message in the current message vector
        for (const auto& message : *message_vector) {
            std::print("{}", message); // Print it
        }
    }
}

void ExploreState::build_map(const Game& game) const {
    for (const auto& row : game.get_world().overworld_occupants_) {
        std::string new_row_string;
        for (const auto& tile : row) {
            if (tile == nullptr) {
                // new_row_string += '𖠰';
                new_row_string += '*';
            } else if (dynamic_cast<PlayerCharacter*>(tile)) {
                // new_row_string += '𖨆';
                new_row_string += '@';
            } else if (dynamic_cast<Enemy*>(tile)) {
                // new_row_string += '𖢥';
                new_row_string += '#';
            } else {
                new_row_string += '!';
            }
        }
        std::print("{}\n", new_row_string);
    }
}

void ExploreState::handle_input(Game& game, std::string_view input) {
    const std::string choice = normalize_input(input);

    if (choice == "battle" || choice == "fight" || choice == "1") {
        game.request_state_change(std::make_unique<BattleState>());
        return;
    }

    if (choice == "quit" || choice == "exit") {
        game.quit();
        return;
    }

    if (choice == "w" || choice == "W") {
        move(game, Action::MOVE_UP);
    }

    if (choice == "a" || choice == "A") {
        move(game, Action::MOVE_LEFT);
    }

    if (choice == "s" || choice == "S") {
        move(game, Action::MOVE_DOWN);
    }

    if (choice == "d" || choice == "D") {
        move(game, Action::MOVE_RIGHT);
    }

    action_menu_.clear();
    action_menu_.emplace_back("Choose an action:\n1. battle\n2. quit\n");
}

void ExploreState::move([[maybe_unused]] Game& game, Action) {
    auto the_x_pos = game.get_world().player_.get_x_pos()
}

void ExploreState::clear_message_vectors() {
    overworld_map_.clear();
    status_display_.clear();
    action_menu_.clear();
}
/*
𖨆
𖠋
𖢥
𖠰

· · ─ ·🗺· ─ · ·
── ⋆⋅𝐖𝐄𝐋𝐂𝐎𝐌𝐄⋅⋆ ──
· · ─ ·𖥸· ─ · ·


﴾
﴿
 ⋆༺𓆩☠︎︎𓆪༻⋆
𝐘𝐀'𝐋𝐋 𝐃𝐈𝐄𝐃
𝐆𝐀𝐌𝐄 𝐎𝐕𝐄𝐑
 𓆩༺ ☠︎︎ ༻𓆪
*/
