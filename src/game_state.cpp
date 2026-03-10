// Abstract function GameState with pure virtal functions
class GameState {
  public:
    virtual void render() = 0;
    virtual void handle_input() = 0;
    virtual void update() = 0;
};