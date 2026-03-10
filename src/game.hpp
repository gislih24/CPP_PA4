#ifndef GAME_HPP
#define GAME_HPP

#include "game_state.hpp"
#include <memory>

class Game {
  public:
    std::unique_ptr<GameState> state;
};

#endif // GAME_HPP