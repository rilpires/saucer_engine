#ifndef SCRIPT_H
#define SCRIPT_H

#include <unordered_map>
#include <vector>
#include <string.h>
#include "saucer_object.h"
#include "saucer_type_traits.h"

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


#define REGISTER_LUA_MEMBER_FUNCTION(C,F)\
    LuaEngine::register_function(#C,#F, LuaEngine::to_lua_cfunction< function_member_unconstantizer<decltype(&C::F)>::type >::generate_lambda< function_ret_type<decltype(&C::F)>::type ,&C::F>() );
#define REGISTER_LUA_NESTED_STATIC_FUNCTION(C,F)\
    LuaEngine::register_function(#C,#F, LuaEngine::to_lua_cfunction< decltype(C::F) >::generate_lambda< function_ret_type<decltype(C::F)>::type ,C::F>() );
#define REGISTER_LUA_GLOBAL_FUNCTION(F_NAME,F)\
    LuaEngine::register_function(F_NAME, LuaEngine::to_lua_cfunction< decltype(F) >::generate_lambda< function_ret_type<decltype(F)>::type ,F>() );
#define REGISTER_LUA_CONSTANT(ENUM_NAME,INDEX_NAME,VALUE)\
    LuaEngine::register_constant(#ENUM_NAME,#INDEX_NAME,VALUE)




class Sprite;
class Scene;
class LuaEngine {
    friend class SceneNode;
    private:
        static lua_State*       ls;
        static int              kb_memory_used;
        static int              kb_memory_threshold;
        static size_t           chunk_reader_offset;

        static std::unordered_map< std::string , std::unordered_map< std::string , lua_CFunction > > nested_functions_db;
        static std::unordered_map< std::string , std::unordered_map< std::string , int > > constants;
        static std::unordered_map< std::string , lua_CFunction > global_functions_db;
        
        static const char*      chunk_reader( lua_State* ls , void* data , size_t* size );
        static void             create_global_env();
        static void             change_current_actor_env( SceneNode* new_actor );
        static void             push_actor_table(  lua_State* ls , SceneNode* actor);
        static void             push_actor_userdata(   lua_State* ls , SceneNode* actor);
        static void             describe_stack();
        static void             print_error( int err , LuaScriptResource* script );
        static lua_CFunction    recover_nested_function( std::string class_name , std::string function_name );
        static lua_CFunction    recover_global_function( std::string function_name );

        template< typename C >
        static lua_CFunction    recover_nested_function( std::string function_name );

    protected:
        static SceneNode*       current_actor;
    
    public:
        static void             initialize();
        static void             finish();



        static void             execute_callback( const char* callback_name , SceneNode* actor );
        template <typename T_arg1 >
        static void             execute_callback( const char* callback_name , SceneNode* actor , T_arg1 arg1 );

        static void             create_actor_env( SceneNode* new_actor );
        static void             register_constant( std::string enum_name , std::string index_name , int i );
        static void             register_function( std::string global_function_name , lua_CFunction f );
        static void             register_function( std::string class_name , std::string function_name , lua_CFunction f );

        template< typename T >
        static lua_CFunction    create_lua_constructor();    
        

        // push signature for std::vectors (const vector& only!)
        template< typename T , typename value_type = typename std::enable_if< is_vector<T>::value , typename is_vector<T>::value_type >::type >
        static void             push( lua_State* ls , T v );

        // push signature for SaucerObject's inherited classes
        template< typename T , class = typename std::enable_if< !is_vector<T>::value && std::is_base_of<SaucerObject, typename std::remove_pointer<T>::type >::value >::type , typename = void >
        static void             push( lua_State* , T obj );

        // push signature for non SaucerObject's inherited classes
        template< typename T , class = typename std::enable_if< !is_vector<T>::value && !std::is_base_of<SaucerObject, typename std::remove_pointer<T>::type >::value >::type  , typename = void , typename = void >
        static void             push( lua_State* , T obj );


        // pop signature for saucer objects , a.k.a pointers... (except for inputevent, they should be specialized)
        template< typename T , class = typename std::enable_if< std::is_pointer<T>::value >::type >
        static T                pop( lua_State* );

        // pop signature for types that is passed by value
        template< typename T , class = typename std::enable_if< !std::is_pointer<T>::value >::type , typename=void  >
        static T                pop( lua_State* );
        
        // By default, the metatable of a type will only have it's functions members, in __index
        template< typename T >
        static void             push_metatable( lua_State* ls );
        
        template< typename F > 
        struct to_lua_cfunction;

};

template<>
lua_CFunction    LuaEngine::recover_nested_function<SaucerObject>( std::string function_name );

template<> void     LuaEngine::push( lua_State* ls , bool b );
template<> void     LuaEngine::push( lua_State* ls , std::string s);
template<> void     LuaEngine::push( lua_State* ls , double n );
template<> void     LuaEngine::push( lua_State* ls , float n );
template<> void     LuaEngine::push( lua_State* ls , int n );
template<> void     LuaEngine::push( lua_State* ls , short n );
template<> void     LuaEngine::push( lua_State* ls , SaucerId n );
template<> void     LuaEngine::push( lua_State* ls , Vector2 v );
template<> void     LuaEngine::push( lua_State* ls , Vector3 v );
template<> void     LuaEngine::push( lua_State* ls , Color v );
template<> void     LuaEngine::push( lua_State* ls , Input::InputEvent* r );

template<> lua_CFunction    LuaEngine::create_lua_constructor<Color>( );
template<> lua_CFunction    LuaEngine::create_lua_constructor<Vector2>( );
template<> lua_CFunction    LuaEngine::create_lua_constructor<Vector3>( );

template<> int                  LuaEngine::pop( lua_State* ls );
template<> float                LuaEngine::pop( lua_State* ls );
template<> short                LuaEngine::pop( lua_State* ls );
template<> double               LuaEngine::pop( lua_State* ls );
template<> bool                 LuaEngine::pop( lua_State* ls );
template<> std::string          LuaEngine::pop( lua_State* ls );
template<> Input::InputEvent*   LuaEngine::pop(lua_State* ls );

template<> void     LuaEngine::push_metatable<SceneNode>( lua_State* ls );



#include "lua_engine.tpp"

#endif