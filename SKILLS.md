# Skills Demonstrated in Saucer Engine

This document summarizes the technical skills and concepts applied throughout this project.

---

## Systems Programming & C++

- **C++11** – Extensive use of modern features: range-based `for`, lambdas, `auto`, `nullptr`, `constexpr`, variadic templates, `std::unordered_map`, `std::vector`, `std::list`, and move semantics.
- **Template metaprogramming** – Custom SFINAE-based `saucer_type_traits.h`, template specializations for Lua type push/pop, and compile-time class registration via `REGISTER_SAUCER_OBJECT`.
- **Macro metaprogramming** – Property binding macros (`PROPERTY_VEC2`, `PROPERTY_FLOAT`, …), component registration macros (`REGISTER_AS_COMPONENT`, `REGISTER_COMPONENT_HELPERS`), and Lua function registration macros (`REGISTER_LUA_MEMBER_FUNCTION`).
- **Object-oriented design** – Inheritance hierarchies (SaucerObject → Component → RenderObject → Sprite / LabelRect / PatchRect), virtual dispatch, and rule-of-three/five patterns.
- **Memory management** – Manual heap allocation and deallocation for scene nodes and components, ownership semantics via a centralized object registry keyed by `SaucerId`.

---

## Graphics Programming (OpenGL)

- **OpenGL 3.0** via GLEW 2.1 – Buffer objects (VBO, VAO, EBO), vertex attribute configuration, depth testing, and alpha blending.
- **GLSL shaders** – Custom vertex and fragment shaders supporting model/view/projection transforms, UV mapping, color modulation, texture sampling, and alpha-mask mode.
- **Sprite batching** – A single mapped vertex buffer (`GL_STREAM_DRAW`) accumulates all render data each frame before a single draw call, minimizing GPU state changes.
- **Render pipeline** – Z-sorting of the scene tree, camera transform inversion, and viewport remapping to normalized device coordinates.
- **Font rendering** – FreeType 2 rasterization integrated with OpenGL textures and per-glyph UV atlasing.

---

## Game Engine Architecture

- **Entity-Component System (ECS)** – `SceneNode` (entity) + `Component` (behavior/render component), with a static multi-map allowing fast per-node component lookups.
- **Scene graph** – Hierarchical `SceneNode` tree with accumulated transform (position, rotation, scale), inherited modulate color, and Z-ordering.
- **Resource management** – Reference-counted resource system with lazy loading, a global path-to-ID map, and support for both loose files and a compressed binary package.
- **Asset bundling** – zlib-based deflate compression of all project assets into a single `package.data` binary with an embedded table-of-contents; accessible from both CLI (`./saucer pack`) and the in-editor "Pack Resources" button.
- **Serialization** – YAML-based serialization/deserialization of scene templates (`.node`) and project configuration (`.config`) via yaml-cpp.

---

## Scripting Integration (Lua)

- **Lua 5.0.3 embedding** – Full Lua state lifecycle management (open, execute, reset, close).
- **Lua ↔ C++ binding** – Manual C API: `lua_pushcfunction`, metatables (`__index`, `__newindex`, `__eq`), userdata for SaucerObjects and value types (Vector2, Color, Rect), and a per-node virtual environment table for script isolation.
- **Callback dispatch** – Frame, input, physics, and lifecycle events dispatched to registered Lua functions (`_frame_start`, `_input`, `_entered_tree`, `_exiting_tree`, `_collision_start`, `_collision_end`).
- **Lua profiler** – In-editor Lua heap monitoring and environment variable inspector.

---

## Physics & Collision

- **Box2D 2.4.1 integration** – `CollisionBody` component wraps Box2D rigid bodies; dynamic, static, and kinematic types; contact listener dispatches collision start/end events to Lua scripts.

---

## Audio

- **OpenAL-soft 1.21.0** – 2D audio playback via `SamplePlayer` (one-shot WAV/OGG) and `StreamPlayer` (streamed OGG); per-frame streaming update loop.
- **Codec support** – WAV decoding via AudioFile 1.0.8; OGG/Vorbis decoding via libogg 1.3.4 + libvorbis 1.3.7.

---

## Editor (Dear ImGui)

- **Integrated editor** – Built only in debug builds via `#ifdef SAUCER_EDITOR`; completely excluded from release binaries.
- **ImGui 1.79** – Dockable windows: scene tree, node inspector, project config, render preview, resource explorer, Lua editor, Lua profiler, engine profiler, and console.
- **Drag-and-drop** – Resource paths dragged from the resource explorer and dropped onto scene tree nodes.
- **Play-in-editor** – Duplicate-and-run the current scene tree in-place, then restore the original on stop.
- **Asset packing from editor** – "Pack Resources" menu item in the Project Configuration window triggers the same compression pipeline used by the CLI.

---

## Build System & Tooling

- **CMake** (3.5+) – Modular `CMakeLists.txt` with optional features (`SAUCER_BUILD_EDITOR`, `SAUCER_LINK_COMPONENTS`), platform-specific MSVC vs. GCC/Clang paths, and automatic component subdirectory discovery.
- **Git submodules** – All third-party libraries managed as git submodules for reproducible builds.
- **Python scripting** – `toc_builder.py` utility script that walks the `res/` directory and emits a flat text table-of-contents used by the packing pipeline.
- **Cross-platform** – Builds on Windows (MSVC, with `/SUBSYSTEM:WINDOWS` in release) and Linux/macOS (GCC/Clang).

---

## Image Processing

- **libPNG 1.6.37** – Loading PNG textures into OpenGL via raw pixel data; support for RGB and RGBA color formats.
- **Sprite sheets** – `h_frames` / `v_frames` / `frame_index` properties on the Sprite component for atlas-based animation.

---

## Software Design Patterns

| Pattern | Where Used |
|---|---|
| Singleton (static class) | `Engine`, `RenderEngine`, `LuaEngine`, `Input`, `ResourceManager` |
| Observer / Event queue | Input event queue dispatched per-frame to scene nodes |
| Composite | Scene tree of `SceneNode` objects |
| Factory | `REGISTER_COMPONENT_HELPERS` macro generating `create_X` / `get_X` per component type |
| Template method | `RenderObject::generate_render_data()` overridden by each render component |
| Registry | `SaucerObject` global ID map; `__class_bind_methods` static initializer list |
| Strategy | Lua scripts as interchangeable behavior attached to `SceneNode` |
