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


#define REGISTER_LUA_MEMBER_FUNCTION(C,F)\
    LuaEngine::register_function(#C,#F, LuaEngine::to_lua_cfunction< function_member_unconstantizer<decltype(&C::F)>::type >::generate_lambda< function_ret_type<decltype(&C::F)>::type ,&C::F>() );
#define REGISTER_LUA_STATIC_FUNCTION(C,F)\
    LuaEngine::register_function(#C,#F, LuaEngine::to_lua_cfunction< decltype(C::F) >::generate_lambda< function_ret_type<decltype(C::F)>::type ,C::F>() );


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
        static lua_CFunction    recover_cfunction( std::string class_name , std::string function_name );

    protected:
        static SceneNode*       current_actor;
    
    public:
        static void             initialize();
        static void             finish();

        static void             execute_frame_start( SceneNode* actor );
        static void             execute_input_event( SceneNode* actor );
        static void             create_actor_env( SceneNode* new_actor );
        static void             register_function( std::string class_name , std::string function_name , lua_CFunction f );

        template<typename T>
        static lua_CFunction    create_lua_constructor( lua_State* );    
        
        template<typename T>
        static void             push( lua_State* , T );

        template<typename T>
        static T                pop( lua_State* );
        
        template< typename F > 
        struct to_lua_cfunction;

};




template<typename T> 
struct function_member_unconstantizer;
template<typename ret_type , typename class_type , typename ... args_type> 
struct function_member_unconstantizer<ret_type(class_type::*)(args_type ...)>{
    using type = ret_type(class_type::*)(args_type ...);
};
template<typename ret_type , typename class_type , typename ... args_type> 
struct function_member_unconstantizer<ret_type(class_type::*)(args_type ...) const>{
    using type = ret_type(class_type::*)(args_type ...);
};
template<typename ret_type , typename ... args_type> 
struct function_member_unconstantizer<ret_type(args_type ...)>{
    using type = ret_type(args_type ...);
};

template<typename T> 
struct function_ret_type;
template<typename ret_type , typename class_type , typename ... args_type> 
struct function_ret_type<ret_type(class_type::*)(args_type ...)>{
    using type = ret_type;
};
template<typename ret_type , typename class_type , typename ... args_type> 
struct function_ret_type<ret_type(class_type::*)(args_type ...) const>{
    using type = ret_type;
};
template<typename ret_type , typename ... args_type> 
struct function_ret_type<ret_type(args_type ...)>{
    using type = ret_type;
};


template<typename T, class = typename std::enable_if< !std::is_pointer<T>::value >::type >
T* to_pointer( T& obj ){ return &obj; };
template<typename T, class = typename std::enable_if< !std::is_pointer<T>::value >::type >
T* to_pointer( T* obj){ return obj; };

template<typename T> struct to_used_type;

#define SAUCER_USE_BY_VALUE(T)\
template<> struct to_used_type<T>{ using type = T; };
#define SAUCER_USE_BY_REFERENCE(T)\
template<> struct to_used_type<T>{ using type = T*; }

SAUCER_USE_BY_VALUE(int);
SAUCER_USE_BY_VALUE(size_t);
SAUCER_USE_BY_VALUE(short);
SAUCER_USE_BY_VALUE(Color);
SAUCER_USE_BY_VALUE(Vector2);
SAUCER_USE_BY_VALUE(Vector3);
SAUCER_USE_BY_VALUE(Transform);

SAUCER_USE_BY_REFERENCE(SceneNode);
SAUCER_USE_BY_REFERENCE(Scene);
SAUCER_USE_BY_REFERENCE(Resource);
SAUCER_USE_BY_REFERENCE(ImageResource);
SAUCER_USE_BY_REFERENCE(LuaScriptResource);

template< typename R >
struct LuaEngine::to_lua_cfunction<R()>{
    using function_type = R();
    
    template< typename ret_type , function_type f , class = typename std::enable_if< !std::is_same<ret_type,void>::value >::type >
    static lua_CFunction generate_lambda(){
        return []( lua_State* ls ){
            LuaEngine::push<R>( ls , f() ); return 1;
        };
    }
    template< typename ret_type , function_type f , class = typename std::enable_if< std::is_same<ret_type,void>::value >::type , class = int >
    static lua_CFunction generate_lambda(){
        return []( lua_State* ls ){
            f(); return 0;
        };
    }
};
template< typename R , typename T_arg1 >
struct LuaEngine::to_lua_cfunction<R(T_arg1)>{
    using function_type = R(T_arg1);
    
    template< typename ret_type , function_type f , class = typename std::enable_if< !std::is_same<ret_type,void>::value >::type >
    static lua_CFunction generate_lambda(){
        return []( lua_State* ls ){
            T_arg1 arg1 = LuaEngine::pop<T_arg1>(ls);
            LuaEngine::push<R>( ls , f(arg1) ); return 1;
        };
    }
    template< typename ret_type , function_type f , class = typename std::enable_if< std::is_same<ret_type,void>::value >::type , class=int >
    static lua_CFunction generate_lambda(){
        return []( lua_State* ls ){
            T_arg1 arg1 = LuaEngine::pop<T_arg1>(ls);
            f( arg1 ); return 0;
        };
    }
};

template< typename R , typename C >
struct LuaEngine::to_lua_cfunction<R(C::*)()>{
    using function_type = R(C::*)();
    using const_function_type = R(C::*)() const;
    using class_type = typename to_used_type<C>::type;

    template< typename ret_type , function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            (obj_pointer->*f)();
            return 0;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            (obj_pointer->*f)();
            return 0;
        };
    }
    template< typename ret_type , function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            R ret = (obj_pointer->*f)();
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            R ret = (obj_pointer->*f)();
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
};
template< typename R , typename C , typename T_arg1 >
struct LuaEngine::to_lua_cfunction<R(C::*)(T_arg1)>{
    using function_type = R(C::*)(T_arg1);
    using const_function_type = R(C::*)(T_arg1) const;
    using class_type = typename to_used_type<C>::type;

    template< typename ret_type , function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            (obj_pointer->*f)(arg1);
            return 0;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            (obj_pointer->*f)(arg1);
            return 0;
        };
    }
    template< typename ret_type , function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            R ret = (obj_pointer->*f)(arg1);
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            R ret = (obj_pointer->*f)(arg1);
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
};
template< typename R , typename C , typename T_arg1, typename T_arg2 >
struct LuaEngine::to_lua_cfunction<R(C::*)(T_arg1,T_arg2)>{
    using function_type = R(C::*)(T_arg1,T_arg2);
    using const_function_type = R(C::*)(T_arg1,T_arg2) const;
    using class_type = typename to_used_type<C>::type;

    template< typename ret_type , function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg2      arg2    = LuaEngine::pop<T_arg2>(ls);
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            (obj_pointer->*f)(arg1,arg2);
            return 0;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg2      arg2    = LuaEngine::pop<T_arg2>(ls);
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            (obj_pointer->*f)(arg1,arg2);
            return 0;
        };
    }
    template< typename ret_type , function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg2      arg2    = LuaEngine::pop<T_arg2>(ls);
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            R ret = (obj_pointer->*f)(arg1);
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C* obj_pointer = to_pointer<C>(obj);
            R ret = (obj_pointer->*f)(arg1);
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
};

#endif