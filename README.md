# saucer_engine
Game engine made with C++11, using OpenGL 3.0 as rendering back-end.
The focus on this engine was primarily for learning graphics programming, and then some low level game development in general. I'll be trying to do the best pratices to make this engine in a very modular way, so anyone can feel free to mess with some core features like physics.

Currently I'm developing this engine alongside a simple game I'm making with it. My plan is to develop this engine until then.

Documentation is being created with Doxygen. (Incomplete)

Soon more informations about compiling.
 
Currently, apart from C++ STL libs, these are all third-party dependencies:
- [GLFW 3.4](https://github.com/glfw/glfw), for window management and graphics context.
- [GLEW 2.1](https://github.com/nigels-com/glew), runtime checkups and bindings for OpenGL drivers.
- [libPNG 1.6.37](http://www.libpng.org/pub/png/libpng.html), reading png files.
- [Lua script 5.0.3](https://www.lua.org/home.html), for scripting game actors and logic.
- [Box2D 2.4.1](https://github.com/erincatto/box2d/tree/v2.4.1), for physics and collisions logic.
- [OpenAL-soft 1.21.0](https://github.com/kcat/openal-soft/tree/openal-soft-1.21.0), for audio rendering.
- [AudioFile 1.0.8](https://github.com/adamstark/AudioFile/tree/1.0.8) , for loading .WAV files.
- [FreeType 2.10.4](https://www.freetype.org/index.html), for loading fonts.
- [Dear ImGui 1.79](https://github.com/ocornut/imgui/tree/v1.79), for graphic editor interface

Nexts TO-DO's for sure:
- A very basic editor for setting up scenes, since scripts is being very verbose in this task. This come up suddenly because I tried Dear ImGui and it was very easy to construct an editor interface. Open source community is awesome!
- Saving Scene/SceneNodes templates in files, JSON or YAML maybe
- Networking module
- Packing resources to a proper release build
- More audio formats, cursor and possibly positioning("3D") sound.

Possibly TO-DO's:
- Imports for some popular tools (Aseprite, Tiled, ... )
