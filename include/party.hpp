#pragma once

#include "player_character.hpp"
#include <memory>
#include <vector>

class Party {
  public:
    void add_member(std::unique_ptr<PlayerCharacter> member);
    std::vector<std::unique_ptr<PlayerCharacter>>& members();
    const std::vector<std::unique_ptr<PlayerCharacter>>& members() const;

  private:
    std::vector<std::unique_ptr<PlayerCharacter>> members_{};
};
