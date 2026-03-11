#pragma once

#include <cctype>
#include <string>
#include <string_view>

class Game;

class GameState {
  public:
    virtual ~GameState() = default;

    virtual void on_enter(Game& game) = 0;
    virtual void render(const Game& game) const = 0;
    virtual void handle_input(Game& game, std::string_view input) = 0;

  protected:
    /**
     * @brief Normalizes the input by trimming leading and trailing whitespace
     * and converting all characters to lowercase.
     * @param input The input string to normalize.
     * @return The normalized string.
     */
    std::string normalize_input(std::string_view input) const {
        std::size_t start = 0;
        while (start < input.size() &&
               std::isspace(static_cast<unsigned char>(input[start]))) {
            ++start;
        }

        std::size_t end = input.size();
        while (end > start &&
               std::isspace(static_cast<unsigned char>(input[end - 1]))) {
            --end;
        }

        std::string result;
        result.reserve(end - start);

        for (std::size_t i = start; i < end; ++i) {
            result.push_back(static_cast<char>(
                std::tolower(static_cast<unsigned char>(input[i]))));
        }

        return result;
    }
};
