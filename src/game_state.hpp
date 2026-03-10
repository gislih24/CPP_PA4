#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

class GameState {
  public:
    virtual ~GameState() = default;

    virtual void render() = 0;
    virtual void handle_input() = 0;
    virtual void update() = 0;
};

#endif // GAME_STATE_HPP