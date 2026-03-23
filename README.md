# Metroidvania

A 2D side-scrolling action platformer built in C++17 with SFML 3.
Primary design reference: Castlevania: Symphony of the Night.

## Status

Early development. Core engine systems running.
Player cube moves on a tile floor with gravity and AABB collision.

## Requirements

- Visual Studio 2026 (x64)
- SFML 3.0.x — https://www.sfml-dev.org/download/

## Setup

1. Clone the repo
2. Download SFML 3.0.x (Visual C++ / x64)
3. Place SFML in `ProjectMetroidvania/SFML/`
4. Open `ProjectMetroidvania.sln` in Visual Studio
5. Build and run (Debug x64 or Release x64)

## Project Structure
```
ProjectMetroidvania/
├── Action.h              # Action enum - input bindings
├── Input.h / .cpp        # Stateful action-mapped input with 10-frame buffer
├── Entity.h / .cpp       # Base class for all dynamic objects
├── Tile.h / .cpp         # 128x128 tile, Stone/Platform/Slope/Empty
├── TileMap.h / .cpp      # Flat tile grid, spatial collision queries
├── AnimationState.h      # Full SotN-inspired animation state set
├── Animator.h / .cpp     # Clip-based animator with interrupt rules
├── AbilitySet.h          # Player ability slots (stub)
├── Player.h / .cpp       # Movement, gravity, AABB collision, coyote time
├── Game.h / .cpp         # Main loop, systems owner, test level
└── main.cpp              # Entry point
```

## Controls

| Key | Action |
|-----|--------|
| A / D | Move left / right |
| S | Crouch |
| Space | Jump |
| J | Attack (primary) |
| K | Attack (secondary) |
| U / I | Ability slots |
| E | Interact |
| Escape | Quit |

## Architecture Notes

- Input is action-mapped — nothing touches raw SFML events except `Input`
- Input buffer depth: 10 frames (~166ms) — tunable via `k_inputBufferDepth`
- Coyote time: 6 frames — tunable via `k_coyoteFrames`
- Tiles are 128x128px — tunable via `k_tileSize`
- Collision is AABB, resolved on shortest overlap axis
- Animator supports interrupt rules and forced state transitions

## License

Private — all rights reserved.