# Metroidvania

A 2D side-scrolling action platformer built in C++17 with SFML 3.
Primary design reference: Castlevania: Symphony of the Night.

## Status

Early development. Core engine systems running.
Player moves, jumps and collides on a tile-based world with a following camera.
In-game level editor (DevMode) operational.

## Requirements

- Visual Studio 2026 (x64)
- SFML 3.0.x — https://www.sfml-dev.org/download/
- A TTF font at `assets/fonts/default.ttf` (Arial from C:\Windows\Fonts\ works fine)

## Setup

1. Clone the repo
2. Download SFML 3.0.x (Visual C++ / x64)
3. Place SFML in `ProjectMetroidvania/SFML/`
4. Copy a .ttf font to `assets/fonts/default.ttf`
5. Open `ProjectMetroidvania.sln` in Visual Studio
6. Build and run (Debug x64 or Release x64)

## Project Structure
```
ProjectMetroidvania/
├── Action.h              # Action enum - input bindings
├── Input.h / .cpp        # Stateful action-mapped input with 10-frame buffer
├── Entity.h / .cpp       # Base class for all dynamic objects
├── Tile.h / .cpp         # 64x64 tile, Stone/Platform/Slope/Empty
├── TileMap.h / .cpp      # Flat tile grid, spatial collision queries
├── AnimationState.h      # Full SotN-inspired animation state set
├── Animator.h / .cpp     # Clip-based animator with interrupt rules
├── AbilitySet.h          # Player ability slots (stub)
├── Player.h / .cpp       # Movement, gravity, AABB collision, coyote time
├── Camera.h / .cpp       # Follow camera, room bounds, Free/Locked/Transition modes
├── DevMode.h / .cpp      # In-game level editor, three layer painting
├── Game.h / .cpp         # Main loop, systems owner, layer management
└── main.cpp              # Entry point
```

## Controls

### Gameplay
| Key | Action |
|-----|--------|
| A / D | Move left / right |
| S | Crouch |
| Space | Jump |
| J | Attack (primary) |
| K | Attack (secondary) |
| U / I | Ability slots |
| E | Interact |
| F1 | Toggle DevMode |
| Escape | Quit |

### DevMode (Level Editor)
| Input | Action |
|-------|--------|
| WASD | Move camera |
| Left click | Place tile |
| Right click | Erase tile |
| Scroll wheel | Cycle tile type |
| Tab | Cycle layer forward |
| Shift + Tab | Cycle layer backward |
| F1 | Return to gameplay |

## Architecture Notes

- Input is action-mapped — nothing touches raw SFML events except `Input`
- Input buffer depth: 10 frames (~166ms) — tunable via `k_inputBufferDepth`
- Coyote time: 6 frames — tunable via `k_coyoteFrames`
- Tile size: 64x64px — tunable via `k_tileSize`
- Three render layers: Background (no collision), Midground (collision), Foreground (no collision)
- Camera supports Free / Locked / Transition modes driven by room metadata
- Collision is AABB on midground layer only — platform one-way logic is a future pass
- Animator supports interrupt rules and forced state transitions (Hurt/Die)

## Known Issues

- Platform tiles have no collision yet — one-way logic is a planned pass
- No asset pipeline yet — all visuals are placeholder shapes
- Level edits are not saved between sessions — save/load system is next

## License

All Rights Reserved — Copyright (c) 2026