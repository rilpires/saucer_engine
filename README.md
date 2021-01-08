# saucer_engine
Game engine made with C++11, using OpenGL 3.1.
The focus on this engine was primarily for learning graphics programming, and then some low level game development in general. I'll be trying to do the best pratices to make this engine in a very modular way, so anyone can feel free to mess with some core features like physics.

Currently it is totally experimental. My goal is to ship a simple game with it, which I plan to make use of networking as well. Initially, I plan to develop this engine until then.

Soon more informations about compiling and possibly documentation.
 
Currently, apart from C++ STL libs, these are all third-party dependencies:
- [GLFW 3.4](https://github.com/glfw/glfw), for window management and graphics context.
- [GLEW 2.1](https://github.com/nigels-com/glew), runtime checkups and bindings for OpenGL drivers.
- [libPNG 1.6.37](http://www.libpng.org/pub/png/libpng.html), reading png files.
- [Lua script 5.0.3](https://www.lua.org/home.html), for scripting game actors and logic.
- [Box2D 2.4.1](https://github.com/erincatto/box2d/tree/v2.4.1), for physics and collisions logic.
- [OpenAL-soft 1.21.0](https://github.com/kcat/openal-soft/tree/openal-soft-1.21.0), for audio rendering.
- [AudioFile 1.0.8](https://github.com/adamstark/AudioFile/tree/1.0.8) , for loading .WAV files.

Nexts TO-DO's:
- Clean & organize a little bit renderer stuff
- Saving Scene/SceneNodes templates in files, JSON or YAML maybe
- Networking module
- More audio formats, cursor and possibly positioning("3D") sound.
- Packing resources to a proper export
- Imports for some popular tools (Aseprite, Tiled, ... )
- A very basic editor (probably very very far)
