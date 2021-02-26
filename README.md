# Saucer Engine
Saucer Engine is a 2D Game engine made with C++11, using OpenGL 3.0 as rendering back-end.
The focus on this engine was primarily for learning graphics programming, and then some low level development in general. It evolved a little bit further, with an integrated gui editor, with embedded lua profiling and scene editor ([ImGui](https://github.com/ocornut/imgui) is indeed an amazing library).

It's focus is to be lightweighted and script-friendly, so you don't need to write C++ if you want, although it is simple to add custom C++ components.

[I made this proof-of-concept game with Saucer](https://rilpires.itch.io/1bchess). It's a chess game. It uses a [custom C++ component](https://github.com/rilpires/saucer_chess) to communicate with a external process (any chess engine executable that uses UCI protocol, like Stockfish).

# Dependencies 
Apart from heavy use of C++ STL libs, these below are all third-party dependencies:
- [GLFW 3.3.2(*)](https://github.com/glfw/glfw/tree/3.3.2), for window management and graphics context.
- [GLEW 2.1(*)](https://github.com/nigels-com/glew/tree/glew-2.1.0), runtime checkups and bindings for OpenGL drivers.
- [libPNG 1.6.37](http://www.libpng.org/pub/png/libpng.html), reading png files.
- [Lua script 5.0.3](https://www.lua.org/home.html), for scripting game actors and logic.
- [Box2D 2.4.1(*)](https://github.com/erincatto/box2d/tree/v2.4.1), for physics and collisions logic.
- [OpenAL-soft 1.21.0(*)](https://github.com/kcat/openal-soft/tree/openal-soft-1.21.0), for audio rendering.
- [AudioFile 1.0.8(*)](https://github.com/adamstark/AudioFile/tree/1.0.8) , for loading .wav files.
- [Ogg 1.3.4(*)](https://github.com/xiph/ogg) , for loading .OGG files.
- [Vorbis 1.3.7(*)](https://github.com/xiph/vorbis/tree/v1.3.7), for decoding .OGG files.
- [FreeType 2.10.4](https://www.freetype.org/index.html), for loading fonts.
- [Dear ImGui 1.79(*)](https://github.com/ocornut/imgui/tree/v1.79), for graphic editor interface
- [yaml-cpp 0.6.3(*)](https://github.com/jbeder/yaml-cpp/tree/yaml-cpp-0.6.3), for yaml file parsing/emitting
- [zlib 1.2.11](https://zlib.net/), general purpose data compression library, for packaging resources and used by others libraries (libpng, freetype)

(*) - Included as a git submodule. When not included as submodule, there is a copy of the project's source code.

# Building
Currently, I'm working on building a proper release build (resource packaging). For now, you can easily build the editor executable with CMake. Don't forget to include all git submodules when cloning this repository.

# Documentation
Source code documentation is being created with Doxygen. (Incomplete)
Also, I think a script-API (Lua bindings) would be more useful since it is the subset of C++ functions that is useful for scripting. More on that is yet to come too.

# Possibly nexts TO-DO's (by priority):
- Picking scene node objects with mouse cursor in editor mode.
- More API functions related to physics & collision (velocity, normal vector, etc)
- Networking module
- Integrate a file explorer in editor (will use boost or C++17) 
- Supporting Lua to the most recent version (5.4.2).
- Imports for some popular tools (Aseprite, Tiled, OGMO, ... suggestions accepted! )
