#include "./include/party.hpp"

#include <utility>

void Party::add_member(std::unique_ptr<PlayerCharacter> member) {
    members_.push_back(std::move(member));
}

std::vector<std::unique_ptr<PlayerCharacter>>& Party::members() {
    return members_;
}

const std::vector<std::unique_ptr<PlayerCharacter>>& Party::members() const {
    return members_;
}
