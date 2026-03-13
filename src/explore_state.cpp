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

void ExploreState::render(const Game&) const {
    // For each of the message vectors
    for (const auto* message_vector :
         {&overworld_map_, &status_display_, &action_menu_}) {
        // For each message in the current message vector
        for (const auto& message : *message_vector) {
            std::print("{}", message); // Print it
        }
    }

    std::cout << std::flush;
}

void ExploreState::build_map(const Game& game) {
    overworld_map_.clear();
    overworld_map_.emplace_back("\nOverworld\n");

    const auto& overworld = game.get_world().get_overworld_occupants();
    std::string header_line = "  ";
    if (!overworld.empty()) {
        for (std::size_t col = 0; col < overworld.front().size(); ++col) {
            header_line += std::to_string(col);
            header_line += ' ';
        }
    }
    header_line += '\n';
    overworld_map_.push_back(std::move(header_line));

    int row_index = 0;
    for (const auto& row : overworld) {
        std::string new_row_string;
        new_row_string += std::to_string(row_index);
        new_row_string += ' ';
        for (const auto& tile : row) {
            if (tile == nullptr) {
                new_row_string += ". ";
            } else if (dynamic_cast<PlayerCharacter*>(tile)) {
                new_row_string += "@ ";
            } else if (dynamic_cast<Enemy*>(tile)) {
                new_row_string += "# ";
            } else {
                new_row_string += "? "; // Something went terribly wrong...
            }
        }
        new_row_string += '\n';
        overworld_map_.push_back(std::move(new_row_string));
        ++row_index;
    }
}

void ExploreState::handle_input(Game& game, std::string_view input) {
    const std::string choice = normalize_input(input);

    if (choice == "quit" || choice == "exit" || choice == "q") {
        game.quit();
        return;
    }

    if (choice == "w") {
        move(game, Action::MOVE_UP);
        return;
    }

    if (choice == "a") {
        move(game, Action::MOVE_LEFT);
        return;
    }

    if (choice == "s") {
        move(game, Action::MOVE_DOWN);
        return;
    }

    if (choice == "d") {
        move(game, Action::MOVE_RIGHT);
        return;
    }

    status_message_ = "Use W/A/S/D to move or Q to quit.";
    rebuild_ui(game);
}

void ExploreState::rebuild_ui(const Game& game) {
    clear_message_vectors();
    build_map(game);

    const auto& player = game.get_world().get_player();
    status_display_.emplace_back(std::format(
        "HP: {}/{} | Defeated enemies: {}\n", player.get_hp(),
        player.get_stats().max_hp, game.get_world().defeated_enemies()));
    status_display_.emplace_back(std::format("{}\n", status_message_));
    status_display_.emplace_back("Legend: @ = player, # = enemy, . = empty\n");

    if (game.get_world().get_enemies().empty()) {
        action_menu_.emplace_back(
            "All enemies are defeated. Type Q to quit.\n");
    } else {
        action_menu_.emplace_back("Move with W/A/S/D. Type Q to quit.\n");
    }
    action_menu_.emplace_back("> ");
}

void ExploreState::move(Game& game, Action action) {
    int row_change = 0;
    int col_change = 0;
    std::string direction;

    switch (action) {
    case Action::MOVE_LEFT:
        col_change = -1;
        direction = "left";
        break;
    case Action::MOVE_RIGHT:
        col_change = 1;
        direction = "right";
        break;
    case Action::MOVE_UP:
        row_change = -1;
        direction = "up";
        break;
    case Action::MOVE_DOWN:
        row_change = 1;
        direction = "down";
        break;
    }

    const MoveOutcome outcome =
        game.get_world().try_move_player(row_change, col_change);

    switch (outcome.result) {
    case MoveResult::moved:
        status_message_ = std::format("You move {}.", direction);
        rebuild_ui(game);
        return;
    case MoveResult::enemy_encounter:
        game.request_state_change(std::make_unique<BattleState>(outcome.enemy));
        return;
    case MoveResult::out_of_bounds:
        status_message_ = "You can't leave the map.";
        rebuild_ui(game);
        return;
    case MoveResult::blocked:
        status_message_ = "That path is blocked.";
        rebuild_ui(game);
        return;
    }
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
