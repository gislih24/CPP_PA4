#include "./include/explore_state.hpp"
#include "./include/game.hpp"
#include <print>

void ExploreState::on_enter(Game& game) override {
    status_display_.emplace_back(
        "Against all the evil that Hell can conjure, all the wickedness\n ");
    status_display_.emplace_back(
        "that mankind can produce, we will send unto them... only you.\n");
    status_display_.emplace_back("Rip and tear, until it is done.\n");
}

void ExploreState::render(const Game& game) const {
    // For each of the message vectors
    for (const auto* message_vector :
         {&overworld_map_, &status_display_, &action_menu_}) {
        // For each message in the current message vector
        for (const auto& message : *message_vector) {
            std::print("{}", message); // Print it
        }
    }
    build_map(game);
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
    }
}

// void ExploreState::handle_input(Game& game, std::string_view input)
// override
// {}
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