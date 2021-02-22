#ifndef LUA_ENGINE_TPP
#define LUA_ENGINE_TPP

#include "lua_engine.h"

template< typename C >
lua_CFunction    LuaEngine::recover_nested_function( std::string function_name ){
    lua_CFunction ret = recover_nested_function( C::class_name , function_name );
    if (!ret) ret = recover_nested_function< typename C::parent_type>( function_name );
    return ret;
}

template < typename T_arg1 >
void             LuaEngine::execute_callback( const char* callback_name , SceneNode* actor , T_arg1 arg1 ){
    if( existent_actors.find(actor->get_saucer_id()) == existent_actors.end() ) return;
    SceneNode* old_actor = current_actor;
    change_current_actor_env( actor );
    push<SceneNode*>( ls , actor );
    lua_pushstring( ls , (std::string("_")+std::string(callback_name)).c_str() );
    lua_gettable(ls,-2);
    lua_insert(ls,-2);
    lua_pop(ls,1);
    push(ls,arg1); // Stack now is: [the function][arg1]
    if( !lua_isnil(ls,-2) ){
        int err = lua_pcall(ls,1,0,0);
        if(err)saucer_err("Error during " , callback_name , " callback");
        print_error(err,actor->get_script());
    } else lua_pop(ls,2);
    change_current_actor_env(old_actor);
}



// push signature for std::vectors
template< typename T , typename C1  >
void    LuaEngine::push( lua_State* ls , T v ){
    lua_newtable(ls);
    for( size_t i = 1 ; i <= v.size() ; i++ ){
        lua_pushnumber(ls,i);
        push(ls,v[i-1]);
        lua_settable(ls,-3);
    }
}

// push signature for saucer objects , (a.k.a pointers... except for inputevents, these should be specialized)
template< typename T , class C1  , class C2 >
void    LuaEngine::push( lua_State* ls , T obj ){
    if(!obj){
        lua_pushnil(ls);
    } else {
        *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = obj->get_saucer_id();
        push_metatable< typename std::remove_pointer<T>::type >(ls);
        lua_setmetatable(ls,-2);
    }
}

// push signature for non SaucerObject's inherited classes
template< typename T , class C1  , class C2 , class C3 >
void    LuaEngine::push( lua_State* ls , T obj ){
    saucer_err( "Specialize \"LuaEngine::push\" for this class since it isn't passed by reference, but by copy." );
    UNUSED(obj);
    lua_pushnil(ls);
}

// pop signature for saucer objects , (a.k.a pointers... except for inputevents, these should be specialized)
 template< typename T , class C1  >
T       LuaEngine::pop( lua_State* ls ){
    if( lua_isnil(ls,-1) ){
        lua_pop(ls,1);
        return nullptr;
    } else {
        #ifdef DEBUG
            SAUCER_ASSERT( lua_isuserdata(ls,-1) , "Expecting userdata from Lua stack, got " , lua_typename(ls,-1) , " : " , lua_tostring(ls,-1) );
        #endif
        SaucerId saucer_id = *(SaucerId*)lua_touserdata(ls,-1);
        T ret = static_cast<T>( SaucerObject::from_saucer_id(saucer_id) );
        lua_pop(ls,1);
        return ret;
    }
}

// pop for types that is passed by value.
template< typename T , class C1 , class C2 >
T       LuaEngine::pop( lua_State* ls ){
    T ret = *(T*)lua_touserdata(ls,-1);
    lua_pop(ls,1);
    return ret;
}

// The default metatable that is created...
template< typename T>
void    LuaEngine::push_metatable( lua_State* ls ){
    static bool initialized = false;
    if(!initialized){
        lua_pushstring(ls,"_SAUCER");
        lua_gettable(ls,LUA_GLOBALSINDEX);
        lua_pushstring(ls,"_METATABLES");
        lua_gettable(ls,-2);
        lua_remove(ls,-2);

        lua_pushstring(ls,T::class_name);

        lua_newtable(ls);
        lua_pushstring(ls,"__index");
        lua_pushcfunction(ls,[](lua_State* ls){
            const char* arg = lua_tostring(ls,-1);
            lua_pop(ls,2);
            auto f = LuaEngine::recover_nested_function<T>(arg);
            if(f){
                lua_pushcfunction( ls , f );
                return 1;
            } else {
                saucer_err( arg , " is not a valid function.");
                return 0;
            }
        });
        lua_settable(ls,-3);
            
        lua_settable(ls,-3);
        lua_remove(ls,-1);
        
        initialized = true;
    }

        lua_pushstring(ls,"_SAUCER");
        lua_gettable(ls,LUA_GLOBALSINDEX);
        lua_pushstring(ls,"_METATABLES");
        lua_gettable(ls,-2);
        lua_remove(ls,-2);
        lua_pushstring(ls,T::class_name);
        lua_gettable(ls,-2);
        lua_remove(ls,-2);
}

/*
Be aware of some....
 _________                            __          _             ___        ____    ____                                
|  _   _  |                          [  |        / |_         .' _ '.     |_   \  /   _|                               
|_/ | | \_|.---.  _ .--..--.  _ .--.  | |  ,--. `| |-'.---.   | (_) '___    |   \/   |   ,--.   .---.  _ .--.   .--.   
    | |   / /__\\[ `.-. .-. |[ '/'`\ \| | `'_\ : | | / /__\\  .`___'/ _/    | |\  /| |  `'_\ : / /'`\][ `/'`\]/ .'`\ \ 
   _| |_  | \__., | | | | | | | \__/ || | // | |,| |,| \__., | (___)  \_   _| |_\/_| |_ // | |,| \__.  | |    | \__. | 
  |_____|  '.__.'[___||__||__]| ;.__/[___]\'-;__/\__/ '.__.' `._____.\__| |_____||_____|\'-;__/'.___.'[___]    '.__.'  
                             [__|    ____  ____  ________  _____     _____                                             
                                    |_   ||   _||_   __  ||_   _|   |_   _|                                            
                                      | |__| |    | |_ \_|  | |       | |                                              
                                      |  __  |    |  _| _   | |   _   | |   _                                          
                                     _| |  | |_  _| |__/ | _| |__/ | _| |__/ |                                         
                                    |____||____||________||________||________|    


*/


#define STATIC_METHOD_BOILERPLATE                                                                                                               \
TEMPLATE_SIGNATURE                                                                                                                              \
struct LuaEngine::to_lua_cfunction<FUNCTION_SIGNATURE>{                                                                                         \
    using function_type = FUNCTION_SIGNATURE;                                                                                                   \
                                                                                                                                                \
    template< typename ret_type , function_type f , class = typename std::enable_if< !std::is_same<ret_type,void>::value >::type >              \
    static lua_CFunction generate_lambda(){                                                                                                     \
        return []( lua_State* ls ){                                                                                                             \
            ARGUMENTS_POPPING                                                                                                                   \
            NON_VOID_RETURN                                                                                                                     \
        };                                                                                                                                      \
    }                                                                                                                                           \
    template< typename ret_type , function_type f , class = typename std::enable_if< std::is_same<ret_type,void>::value >::type , class=int >   \
    static lua_CFunction generate_lambda(){                                                                                                     \
        return []( lua_State* ls ){                                                                                                             \
            ARGUMENTS_POPPING                                                                                                                   \
            VOID_RETURN                                                                                                                         \
        };                                                                                                                                      \
    }                                                                                                                                           \
};


#define TEMPLATE_SIGNATURE template< typename R >
#define FUNCTION_SIGNATURE R()
#define ARGUMENTS_POPPING ;
#define VOID_RETURN f( ); return 0;   
#define NON_VOID_RETURN LuaEngine::push<R>( ls , f() ); return 1;   
STATIC_METHOD_BOILERPLATE
#undef TEMPLATE_SIGNATURE
#undef FUNCTION_SIGNATURE
#undef ARGUMENTS_POPPING 
#undef VOID_RETURN       
#undef NON_VOID_RETURN



#define TEMPLATE_SIGNATURE template< typename R , typename T_arg1 >
#define FUNCTION_SIGNATURE R(T_arg1)
#define ARGUMENTS_POPPING T_arg1 arg1 = LuaEngine::pop<T_arg1>(ls);
#define VOID_RETURN f( arg1 ); return 0;   
#define NON_VOID_RETURN LuaEngine::push<R>( ls , f(arg1) ); return 1;   
STATIC_METHOD_BOILERPLATE
#undef TEMPLATE_SIGNATURE
#undef FUNCTION_SIGNATURE
#undef ARGUMENTS_POPPING 
#undef VOID_RETURN       
#undef NON_VOID_RETURN


#define TEMPLATE_SIGNATURE template< typename R , typename T_arg1 , typename T_arg2 >
#define FUNCTION_SIGNATURE R(T_arg1,T_arg2)
#define ARGUMENTS_POPPING auto arg2 = LuaEngine::pop<T_arg2>(ls); auto arg1 = LuaEngine::pop<T_arg1>(ls);
#define VOID_RETURN f( arg1 , arg2 ); return 0;   
#define NON_VOID_RETURN LuaEngine::push<R>( ls , f(arg1,arg2) ); return 1;   
STATIC_METHOD_BOILERPLATE
#undef TEMPLATE_SIGNATURE
#undef FUNCTION_SIGNATURE
#undef ARGUMENTS_POPPING 
#undef VOID_RETURN       
#undef NON_VOID_RETURN

#define TEMPLATE_SIGNATURE template< typename R , typename T_arg1 , typename T_arg2 , typename T_arg3 >
#define FUNCTION_SIGNATURE R(T_arg1,T_arg2,T_arg3)
#define ARGUMENTS_POPPING auto arg3 = LuaEngine::pop<T_arg3>(ls);auto arg2 = LuaEngine::pop<T_arg2>(ls);auto arg1 = LuaEngine::pop<T_arg1>(ls);
#define VOID_RETURN f( arg1 , arg2 , arg3 ); return 0;   
#define NON_VOID_RETURN LuaEngine::push<R>( ls , f(arg1,arg2,arg3) ); return 1;   
STATIC_METHOD_BOILERPLATE
#undef TEMPLATE_SIGNATURE
#undef FUNCTION_SIGNATURE
#undef ARGUMENTS_POPPING 
#undef VOID_RETURN       
#undef NON_VOID_RETURN


#define CLASS_METHOD_BOILERPLATE                                                                                                                     \
TEMPLATE_SIGNATURE                                                                                                                                   \
struct LuaEngine::to_lua_cfunction<FUNCTION_SIGNATURE>{                                                                                              \
    using function_type = FUNCTION_SIGNATURE;                                                                                                        \
    using class_type = typename to_used_type<C>::type;                                                                                               \
                                                                                                                                                     \
    template< typename ret_type , function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >                    \
    static lua_CFunction   generate_lambda( ){                                                                                                       \
        return []( lua_State* ls ) {                                                                                                                 \
            ARGUMENTS_POPPING                                                                                                                        \
            C& obj_ref = to_ref<C>(obj);                                                                                                             \
            VOID_RETURN                                                                                                                              \
        };                                                                                                                                           \
    }                                                                                                                                                \
    template< typename ret_type , function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >       \
    static lua_CFunction   generate_lambda( ){                                                                                                       \
        return []( lua_State* ls ) {                                                                                                                 \
            ARGUMENTS_POPPING                                                                                                                        \
            C& obj_ref = to_ref<C>(obj);                                                                                                             \
            NON_VOID_RETURN                                                                                                                          \
        };                                                                                                                                           \
    }                                                                                                                                                \
};                                                                                                                                                   \
TEMPLATE_SIGNATURE                                                                                                                                   \
struct LuaEngine::to_lua_cfunction<FUNCTION_SIGNATURE const>{                                                                                        \
    using function_type = FUNCTION_SIGNATURE const;                                                                                                  \
    using class_type = typename to_used_type<C>::type;                                                                                               \
                                                                                                                                                     \
    template< typename ret_type , function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >                    \
    static lua_CFunction   generate_lambda( ){                                                                                                       \
        return []( lua_State* ls ) {                                                                                                                 \
            ARGUMENTS_POPPING                                                                                                                        \
            C& obj_ref = to_ref<C>(obj);                                                                                                             \
            VOID_RETURN                                                                                                                              \
        };                                                                                                                                           \
    }                                                                                                                                                \
    template< typename ret_type , function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >       \
    static lua_CFunction   generate_lambda( ){                                                                                                       \
        return []( lua_State* ls ) {                                                                                                                 \
            ARGUMENTS_POPPING                                                                                                                        \
            C& obj_ref = to_ref<C>(obj);                                                                                                             \
            NON_VOID_RETURN                                                                                                                          \
        };                                                                                                                                           \
    }                                                                                                                                                \
};                                                                                                                     

#define TEMPLATE_SIGNATURE template< typename R , typename C >
#define FUNCTION_SIGNATURE R(C::*)()
#define ARGUMENTS_POPPING auto obj = LuaEngine::pop<class_type>(ls);
#define VOID_RETURN (obj_ref.*f)(); return 0;
#define NON_VOID_RETURN R ret = (obj_ref.*f)(); LuaEngine::push<R>(ls,ret); return 1;
CLASS_METHOD_BOILERPLATE
#undef TEMPLATE_SIGNATURE
#undef FUNCTION_SIGNATURE
#undef ARGUMENTS_POPPING 
#undef VOID_RETURN       
#undef NON_VOID_RETURN

#define TEMPLATE_SIGNATURE template< typename R , typename C , typename T_arg1 >
#define FUNCTION_SIGNATURE R(C::*)(T_arg1)
#define ARGUMENTS_POPPING auto arg1 = LuaEngine::pop<T_arg1>(ls); auto obj = LuaEngine::pop<class_type>(ls);
#define VOID_RETURN (obj_ref.*f)(arg1); return 0;
#define NON_VOID_RETURN R ret = (obj_ref.*f)(arg1); LuaEngine::push<R>(ls,ret); return 1;
CLASS_METHOD_BOILERPLATE
#undef TEMPLATE_SIGNATURE
#undef FUNCTION_SIGNATURE
#undef ARGUMENTS_POPPING 
#undef VOID_RETURN       
#undef NON_VOID_RETURN

#define TEMPLATE_SIGNATURE template< typename R , typename C , typename T_arg1, typename T_arg2 >
#define FUNCTION_SIGNATURE R(C::*)(T_arg1,T_arg2)
#define ARGUMENTS_POPPING auto arg2 = LuaEngine::pop<T_arg2>(ls); auto arg1 = LuaEngine::pop<T_arg1>(ls); auto obj = LuaEngine::pop<class_type>(ls);
#define VOID_RETURN (obj_ref.*f)(arg1,arg2); return 0;
#define NON_VOID_RETURN R ret = (obj_ref.*f)(arg1,arg2); LuaEngine::push<R>(ls,ret); return 1;
CLASS_METHOD_BOILERPLATE
#undef TEMPLATE_SIGNATURE
#undef FUNCTION_SIGNATURE
#undef ARGUMENTS_POPPING 
#undef VOID_RETURN       
#undef NON_VOID_RETURN

#define TEMPLATE_SIGNATURE template< typename R , typename C , typename T_arg1, typename T_arg2, typename T_arg3 >
#define FUNCTION_SIGNATURE R(C::*)(T_arg1,T_arg2,T_arg3)
#define ARGUMENTS_POPPING auto arg3 = LuaEngine::pop<T_arg3>(ls); auto arg2 = LuaEngine::pop<T_arg2>(ls); auto arg1 = LuaEngine::pop<T_arg1>(ls); auto obj = LuaEngine::pop<class_type>(ls);
#define VOID_RETURN (obj_ref.*f)(arg1,arg2,arg3); return 0;
#define NON_VOID_RETURN R ret = (obj_ref.*f)(arg1,arg2,arg3); LuaEngine::push<R>(ls,ret); return 1;
CLASS_METHOD_BOILERPLATE
#undef TEMPLATE_SIGNATURE
#undef FUNCTION_SIGNATURE
#undef ARGUMENTS_POPPING 
#undef VOID_RETURN       
#undef NON_VOID_RETURN

#endif