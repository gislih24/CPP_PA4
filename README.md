# PA4 - Tiny JRPG

This project is a small TUI JRPG written in C++ version 23. It combines a
simple overworld with turn-based battles, save/load support, and a party system
with class-specific abilities.

The assignment requires interface abstraction, and this submission uses
inheritance in two places:
- `GameState` is an abstract base class with concrete states for the main menu,
  exploration, and battle.
- `PlayerCharacter` is a base class with concrete derived classes `Knight` and
  `Wizard`.

## For Players

### What the game includes

- A main menu with `1) New Game`, `2) Load Game`, and `3) Quit`, which you can
  select by typing the corresponding number (or command).
- A 5x5-tile overworld map shown directly in the terminal.
- You can move the player (denoted with `@`) with the `W`, `A`, `S`, `D` keys
  and pressing <kbd>↵ Enter</kbd>.
- Visible enemies on the map (denoted with `#`). Walking into an enemy tile
  starts a battle! 😱
- Turn-based combat with a two-member party:
  - `Knight`: basic attack, a defensive `shield_brace`, and last but certainly
    not least, a `dance` action.
  - `Wizard`: basic attack, `fireball`, and `healing_touch`.
- Your saves are stored in the `saves/` folder.

### Build and run

**Requirements**

- A C++23-capable compiler.
- `make` and a POSIX-like shell environment (e.g., Linux).

Use the provided Makefile:

```bash
make build
make run
```

(Extra commands:)

```bash
make strict
make clean
```

The produced executable `jrpg` can be found in the `/bin` folder.

Tested on Ubuntu 22.04 (WSL2) with `g++-14`.

### Main menu controls

- `1` (or `new`, or `new game`): start a fresh run.
- `2` (or `load`, or `load game`): open the load menu.
- `3` (or `quit`, or `exit`): leave the program.

In the load menu:

- Enter a save number to load it.
- Enter `0` (or `back`, or `b`) to return to the main menu.

### Overworld rules

The map uses these symbols:

- `@` = player
- `#` = enemy
- `.` = empty tile

The player starts in the top-left corner. Three slimes are placed on the map at
the beginning of a new game. You cannot walk off the map, and you cannot move
through occupied tiles except by stepping onto an enemy to initiate combat.
(kinda rude. Hey, maybe this is one of those games where it turns out *you're*
the bad guy? Wouldn't *that* be something?)

Exploration commands:

- `W`, `A`, `S`, `D`: move up↑, left←, down↓, right→.
- `SAVE <name>`: create a save with the chosen name.
- `SAVE`: save again to the currently loaded slot if one exists.
- `Q`, `quit`, or `exit`: leave the game immediately.

Save names are sanitized into lowercase file names inside `saves/`. For
example, `SAVE My Slot` creates a file called `my_slot.txt` in the `saves/`
folder. You can delete files in `saves/` to remove save slots; the folder
itself is expected to exist.

### Battle rules

Battles happen when you move into an enemy tile. The combat system is
turn-based:

- Your party acts first.
- The party contains a `Knight` and a `Wizard`.
- Each living party member acts once per round.
- After both party members have acted, the enemy attacks a random living party
  member.

Available battle actions depend on whose turn it is:

- `1` (or `attack`): basic attack.
- `2`: first class ability.
- `3`: second class ability.
- `0` (or `flee`): run from battle.

Class abilities:

- `Knight`
  - `shield_brace`: gives all allies +2 defence until the enemy finishes its
    next attack.
  - `dance`: flavor action with no gameplay effect.
- `Wizard`
  - `fireball`: deals double the wizard's attack value before defence
    mitigation.
  - `heal`: prompts for an ally target and restores 3 HP.

Important outcomes:

- If the enemy dies, the enemy is removed from the overworld and your character
  moves onto that tile.
- If you flee, you return to exploration without advancing onto the enemy tile.
- If all enemies are defeated, the overworld remains explorable and you can
  still save before quitting.

## For Programmers

### High-level architecture

The program is organized around a main `Game` class that manages the game loop
and state transitions:

- `Game` owns the current `GameState`, the `World`, and the main loop.
- `GameState` defines the common interface:
  - `on_enter(Game&)`
  - `render(const Game&)`
  - `handle_input(Game&, std::string_view)`
- Concrete states:
  - `MainMenuState`
  - `ExploreState`
  - `BattleState`

State changes are not applied immediately. `Game::request_state_change(...)`
stores the next state, and `Game::apply_pending_state_change()` performs the
transition safely between loop stages.

### Main data model

- `World`
  - Owns the persistent run state.
  - Stores the player, the enemy list, the overworld occupancy grid, defeated
    enemy count, and the active save-slot name.
  - Handles map movement, collision checks, save/load, and enemy removal after
    battle.
- `Entity`
  - Base class for anything with stats, HP, and map position.
  - Implements common damage, healing, alive/dead checks, and stat mutation.
- `PlayerCharacter`
  - Derived from `Entity`.
  - Provides the base attack action used by playable units.
- `Knight` and `Wizard`
  - Derived from `PlayerCharacter`.
  - Represent different combat roles and abilities.
- `Enemy`
  - Derived from `Entity`.
  - Currently uses the same base combat model as normal attacks, with per-enemy
    differences driven by stats.
- `Party`
  - Owns the temporary battle party as
    `std::vector<std::unique_ptr<PlayerCharacter>>`.

### File map

- `src/main.cpp`: program entry point.
- `include/game.hpp`, `src/game.cpp`: main loop and state transition control.
- `include/game_state.hpp`: abstract interface for all game states.
- `include/main_menu_state.hpp`, `src/main_menu_state.cpp`: new/load/quit menu
  flow.
- `include/explore_state.hpp`, `src/explore_state.cpp`: overworld rendering,
  movement, and save command handling.
- `include/battle_state.hpp`, `src/battle_state.cpp`: combat loop, party turns,
  enemy turns, and post-battle transitions.
- `include/world.hpp`, `src/world.cpp`: persistent world state, map occupancy,
  save/load parsing, and slot discovery.
- `include/entity.hpp`, `src/entity.cpp`: shared HP/stats/position logic.
- `include/player_character.hpp`, `src/player_character.cpp`: base player
  attack behavior.
- `include/character_classes.hpp`, `src/character_classes.cpp`: `Knight` and
  `Wizard`.
- `include/enemy.hpp`, `src/enemy.cpp`: enemy attack behavior.
- `include/party.hpp`, `src/party.cpp`: battle party container.
- `include/stats.hpp`: simple stat struct with `max_hp`, `attack`, and
  `defence`.

### Design notes

- The overworld is a `5 x 5` grid of raw `Entity*` observers, while ownership
  stays in `World` through the player object and `std::unique_ptr<Enemy>`.
- A new game creates:
  - the player `"The Knight"` with stats `{20 HP, 6 attack, 2 defence}`
  - three slime enemies with progressively increasing stats
- `BattleState` creates a temporary party when combat starts:
  - member 1 is a `Knight` initialized from the current overworld player's HP
    and stats
  - member 2 is a default `Wizard`
- After each player-side update, `BattleState::sync_party_leader(...)` copies
  the first party member's HP back into the overworld player so battle damage
  persists across encounters.
- Save files are plain text with a simple labeled format and a magic header:
  `TINY_JRPG_SAVE_V1`.

### Extension points

This codebase is set up to be extended in a few straightforward ways:

- Add a new player class by deriving from `PlayerCharacter`, then update the
  role/ability dispatch helpers in
  [src/battle_state.cpp](src/battle_state.cpp).
- Add more game screens by deriving from `GameState` and requesting transitions
  through `Game`.
- Add more overworld content by extending `World::reset_new_game()` and
  `World::try_move_player(...)`.
- Add richer combat systems by expanding `Stats`, `Entity`, and `BattleState`.
- Add more persistent data by extending the save/load format in
  [src/world.cpp](src/world.cpp).

### Features added *beyond* a minimal duel

Compared with a minimal one-player-versus-one-enemy battle program, this
submission adds:

- Multiple playable entities in battle.
- Distinct class abilities.
- A visible overworld map.
- Persistent save/load support.
- A multi-state menu/exploration/battle flow.
