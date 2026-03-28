# Snake Demo – Saucer Engine Example Project

A simple Snake game that demonstrates core Saucer Engine features: scene nodes, Lua scripting, input handling, sprite rendering, and resource management.

## Controls

| Key | Action |
|-----|--------|
| ↑ / W | Move up |
| ↓ / S | Move down |
| ← / A | Move left |
| → / D | Move right |
| R | Restart after game over |

## Running the Demo

1. Build the engine first (see the root `README.md`).
2. Copy (or symlink) the built `saucer` executable into this directory (`demo/snake/`).
3. Run the game:
   ```bash
   ./saucer res/project.config
   ```

> **Tip:** Run with the editor binary to open the project in the visual editor and inspect the scene tree live.

## Project Structure

```
demo/snake/
├── res/
│   ├── project.config     # Window size, initial scene path
│   ├── scenes/
│   │   └── main.node      # Root scene node (YAML)
│   ├── scripts/
│   │   └── snake.lua      # All game logic
│   └── sprites/
│       └── cell.png       # 20×20 white square used as sprite (tinted per node)
└── README.md
```

## What This Demo Shows

| Feature | Where |
|---------|--------|
| SceneNode hierarchy | Snake segments and food are child nodes of the root |
| Sprite component | Each cell gets a `create_sprite()` call and a texture |
| Color modulation | `set_self_modulate(Color(...))` tints segments green/red |
| Input handling | `_input(event)` callback reads arrow / WASD keys |
| Frame update loop | `_frame_start(delta)` drives movement timing |
| Dynamic node creation | New segments added with `SceneNode.new()` + `add_child()` |
| Resource loading | `load("res/sprites/cell.png")` fetches the texture resource |
| Scene restart | Replaces root node via `Engine.get_current_scene():set_root_node()` |

## Packing Assets

To bundle all assets into a single `package.data` file for distribution:

1. From **this directory** (`demo/snake/`), run `toc_builder.py` (located at the repository root) to generate the `toc` file:
   ```bash
   python3 ../../toc_builder.py
   ```
2. Run the engine with the `pack` argument:
   ```bash
   ./saucer pack
   ```

Or, if you have the editor build, open the project and click **Pack Resources** in the **Project configuration** window's menu bar.
