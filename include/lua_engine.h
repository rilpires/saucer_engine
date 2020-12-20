#ifndef SCRIPT_H
#define SCRIPT_H

#include "resources/lua_script.h"

#define CHUNK_READER_SIZE 1024

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

class SceneNode;

class LuaEngine {
    private:
        lua_State*      ls;
        int             kb_memory_used;
        int             kb_memory_threshold;

        static size_t   chunk_reader_offset; // Ugh I hate that it should be static. If another LuaEngine should be created, gotta fix it somehow

        void            create_env();
        void            update_env( SceneNode* new_actor );
        void            describe_stack() const;

    public:
        LuaEngine();
        ~LuaEngine();
        
        void    execute( LuaScriptResource* script );

        static const char* chunk_reader( lua_State* ls , void* data , size_t* size );
};

#endif