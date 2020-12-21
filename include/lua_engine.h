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
        static lua_State*       ls;
        static int              kb_memory_used;
        static int              kb_memory_threshold;
        static size_t           chunk_reader_offset;
        static SceneNode*       current_actor;

        static void             create_global_env();
        static void             change_current_actor_env( SceneNode* new_actor );
        static void             describe_stack();
        static void             print_error( int err , LuaScriptResource* script );

    public:
        static void             initialize();
        static void             finish();

        static void             execute_frame_start( SceneNode* actor );
        static void             execute_input_event( SceneNode* actor );
        static void             create_actor_env( SceneNode* new_actor );

        static const char*      chunk_reader( lua_State* ls , void* data , size_t* size );

    private:
        static int              get_position(lua_State* ls);

        static void             lua_push_vector2( lua_State* ls , Vector2 v );
};

#endif