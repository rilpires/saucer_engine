#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "resources.h"
#include "lua_engine.h"
#include <string>

class LuaScriptResource : public Resource {
    friend class LuaEngine;

    private:
        std::string src;
        bool        has_entered_scene = false;
        bool        has_exited_scene = false;
        bool        has_init = false;
        bool        has_frame_start = false;
        bool        has_input = false;
    public:
        LuaScriptResource( std::string filepath );
        ~LuaScriptResource();

        const std::string&          get_src() const {return src;}

        static LuaScriptResource*   get_resource( std::string filepath );

        static void                 bind_methods();


};


#endif