# PA4 - JRPG

## Mermaid diagram of the class structure

```mermaid
---
id: ff385421-a754-4f9d-92ed-7fbd3ef647c2
config:
  layout: elk
---
classDiagram
direction TB
class Game {
    +run() void
    +request_state_change(unique_ptr~GameState~ next_state) void
    +quit() void
    +is_running() bool
    +get_world() World&
    +get_world() const World&
    -apply_pending_state_change() void

    -world_ : World
    -current_state_ : unique_ptr~GameState~
    -pending_state_ : unique_ptr~GameState~
    -game_is_running_ : bool
}
Game "1" *-- "1" GameState
Game "1" *-- "1" World

class GameState {
    <<abstract>>
    +on_enter(Game& game) void*
    +render(const Game& game) void*
    +handle_input(Game& game, string_view input) void*
    #normalize_input(string_view input) string
}
GameState <|-- MainMenuState
GameState <|-- ExploreState
GameState <|-- BattleState
GameState <|-- InventoryState
GameState <|-- GameOverState

class World {
    map : Map
    enemies : vector~Enemy~
    player : PlayerCharacter
}
World "1" *-- "1" Map
World "1" *-- "1" PlayerCharacter
World "1" *-- "1..*" Enemy

class MainMenuState {
    +on_enter(Game& game) void
    +render(const Game& game) void
    +handle_input(Game& game, string_view input) void
    -message_ : string
}

class ExploreState {
    +on_enter(Game& game) void
    +render(const Game& game) void
    +handle_input(Game& game, string_view input) void
}

class BattleState {
    +pc : PlayerCharacter
    +enemy : Enemy
    +line : string
    +in_battle : bool
    +damage_dealt : int
    -combat_log_ : vector~string~
    -status_display_ : vector~string~
    -action_menu_ : vector~string~
    +on_enter(Game& game) void
    +render(const Game& game) void
    +handle_input(Game& game, string_view input) void
    -clear_message_vectors() void
}

class InventoryState {
    +on_enter(Game& game) void
    +render(const Game& game) const void
    +handle_input(Game& game, string_view input) void
}

class GameOverState {
    +on_enter(Game& game) void
    +render(const Game& game) const void
    +handle_input(Game& game, string_view input) void
}

class Map {
}

class PlayerCharacter {
    +target_hp_before : int
    +target_hp_after : int
    +damage_dealt : int
    +attack(Entity& target) int*
}

class Enemy {
    +target_hp_before : int
    +target_hp_after : int
    +damage_dealt : int
    +attack(Entity& target) int*
}

class Entity {
    <<abstract>>
    +name : string
    +Stats : Stats
    +hp : int
    +location : tuple
    +take_damage(int val) void
    +heal(int val) void
    +is_alive() boolean
}
Entity "1" *-- "1" Stats
Entity <|-- PlayerCharacter
Entity <|-- Enemy

class Stats {
    +max_hp : int
    +attack : int
    +defence : int
}
```