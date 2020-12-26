#ifndef SCRIPT_H
#define SCRIPT_H

#include <unordered_map>
#include <vector>
#include <string.h>
#include "saucer_object.h"

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include "resources.h"
#include "resources/image.h"
#include "resources/lua_script.h"

#define CHUNK_READER_SIZE 1024

#include "scene_node.h"

class Scene;
class LuaEngine {
    friend class SceneNode;
    private:
        static lua_State*       ls;
        static int              kb_memory_used;
        static int              kb_memory_threshold;
        static size_t           chunk_reader_offset;

        static std::unordered_map< std::string , std::unordered_map< std::string , lua_CFunction > > c_function_db;

        static const char*      chunk_reader( lua_State* ls , void* data , size_t* size );
        static void             create_global_env();
        static void             change_current_actor_env( SceneNode* new_actor );
        static void             describe_stack();
        static void             print_error( int err , LuaScriptResource* script );

    protected:
        static SceneNode*       current_actor;
    
    public:
        static void             initialize();
        static void             finish();

        static void             execute_frame_start( SceneNode* actor );
        static void             execute_input_event( SceneNode* actor );
        static void             create_actor_env( SceneNode* new_actor );
        static void             register_function( std::string class_name , std::string function_name , lua_CFunction f );

        
    public:
        template<typename T>
        static void             push( lua_State* , T );

        template<typename T>
        static T                pop( lua_State* );
        // template<typename T>
        // static T*               pop( lua_State* );

    public:
        template< typename F >
        struct to_lua_cfunction;

};

#define REGISTER_LUA_FUNCTION(C,F)\
    LuaEngine::register_function(#C,#F, LuaEngine::to_lua_cfunction<decltype(&C::F)>::generate_lambda<(&C::F)>() );

template< typename R , typename C >
struct LuaEngine::to_lua_cfunction<R(C::*)()>{
    using function_type = R(C::*)();
    
    template< function_type f > 
    static lua_CFunction   generate_lambda(){
        return []( lua_State* ls ) {
            C* obj = LuaEngine::pop<C*>(ls);
            R ret = (obj->*f)();
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
};
template< typename R , typename C >
struct LuaEngine::to_lua_cfunction<R(C::*)() const>{
    using function_type = R(C::*)() const;
    
    template< function_type f > 
    static lua_CFunction   generate_lambda(){
        return []( lua_State* ls ) {
            C* obj = LuaEngine::pop<C*>(ls);
            R ret = (obj->*f)();
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
};
template< typename R , typename C , typename T_arg1 >
struct LuaEngine::to_lua_cfunction<R(C::*)(T_arg1)>{
    using function_type = R(C::*)(T_arg1);
    
    template< function_type f > 
    static lua_CFunction   generate_lambda(){
        return []( lua_State* ls ) {
            T_arg1 arg1 = LuaEngine::pop<T_arg1>(ls);
            C* obj = LuaEngine::pop<C*>(ls);
            (obj->*f)(arg1);
            return 0;
        };
    }
};



#endif