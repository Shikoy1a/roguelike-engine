# Rogue Engine (C++ Roguelike)

A small roguelike-style game engine written in modern C++ for learning game architecture, algorithms, and basic engine design.  

This project runs in the terminal and currently supports:

- Procedural dungeon generation (rooms + corridors)
- Player and monsters on a grid map
- Turn-based movement and combat (HP, attack, death)
- Field of View (FoV) & exploration (fog of war)
- Simple A* pathfinding
- Colored console rendering (ANSI colors)
- Simple log system for combat messages
- Immediate keyboard input on Windows (WASD without pressing Enter)

> ✅ Designed as a **portfolio / resume project** to demonstrate C++ skills, algorithms, and engine design.

---

## Features

### Core Gameplay

- **Grid-based dungeon** rendered in the console
- **Player** (`@`) can move with WASD
- **Monsters** (`g`, `o`, …) chase the player using simple AI
- **Combat system**:
  - Player & monsters have `hp`, `maxHp`, `attack`
  - Walking into a monster = attack
  - Monsters can attack the player when adjacent
  - Corpses stay on the map as non-blocking tiles (`x`)

### Procedural Dungeon Generation

- Map is generated at runtime:
  - Multiple random rectangular rooms
  - Corridors connecting room centers (L-shaped)
- First room center is used as the player spawn
- Other rooms can be used to spawn monsters

### Field of View (FoV) & Exploration

- **Circular FoV** around the player with a fixed radius
- Uses a **Bresenham line** algorithm to cast rays from the player to tiles
- Tiles have three states:
  - **Unseen** — never visible, not rendered
  - **Explored** — seen before but currently out of sight (drawn darker)
  - **Visible** — currently within FoV (bright colors)

### Pathfinding

- A simple **A\* pathfinding** implementation:
  - Nodes are walkable tiles (`.`)
  - 4-directional movement
  - Manhattan distance as heuristic
- Can be used for monster AI or user-triggered pathfinding demos.

### Rendering

- Pure **console rendering** using ASCII characters:
  - `#` = wall
  - `.` = floor
  - `@` = player
  - `g`, `o` = monsters
  - `x` = corpse
- Uses **ANSI escape codes** for color:
  - Player: green
  - Monsters: red
  - Walls / floor: grayscale
  - Corpses: purple
  - Explored but not visible tiles: dimmed

> On Windows it is recommended to use **Windows Terminal** or a console that supports ANSI colors.

### Log System

- A small scrolling log at the bottom of the screen:
  - Shows combat messages, deaths, status updates, etc.
  - Keeps the last few lines (e.g. 6 most recent messages)

---

## Project Structure

Approximate layout:

```text
rogue-engine/
├─ main.cpp          # entry point, main game loop
├─ game.hpp          # Game class declaration
├─ game.cpp          # Game implementation (map, FoV, combat, log, dungeon)
├─ entity.hpp        # Entity struct + function declarations
├─ entity.cpp        # Entity-related helpers: collision, movement, find monster
├─ pathfinding.hpp   # Path / A* declarations (if separated)
├─ pathfinding.cpp   # A* pathfinding implementation
├─ .gitignore
└─ README.md
