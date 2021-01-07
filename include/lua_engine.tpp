#ifndef LUA_ENGINE_TPP
#define LUA_ENGINE_TPP

#include "lua_engine.h"


template< typename C >
lua_CFunction    LuaEngine::recover_nested_function( std::string function_name ){
    lua_CFunction ret = recover_nested_function( C::class_name , function_name );
    if (!ret) ret = recover_nested_function< typename C::parent_type>( function_name );
    if (!ret) saucer_err( "Couldn't find function \"" , function_name , "\" in class \"" , C::class_name , "\"." )
    return ret;
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
    saucer_err( "Specialize \"LuaEngine::push\" for this class since it isn't passed by reference, but by copy" )
    lua_pushnil(ls);
    exit(1);
}

// pop signature for saucer objects , (a.k.a pointers... except for inputevents, these should be specialized)
 template< typename T , class C1  >
T       LuaEngine::pop( lua_State* ls ){
    SaucerId saucer_id = *(SaucerId*)lua_touserdata(ls,-1);
    T ret = static_cast<T>( SaucerObject::from_saucer_id(saucer_id) );
    lua_pop(ls,1);
    return ret;
}

// pop for types that is passed by value
template< typename T , class C1 , class C2 >
T       LuaEngine::pop( lua_State* ls ){
    T ret = *(T*)lua_touserdata(ls,-1);
    lua_pop(ls,1);
    return ret;
}

// The default metatable that is created...
template< typename T>
void    LuaEngine::push_metatable( lua_State* ls ){
    lua_newtable(ls);
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls,[](lua_State* ls){
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
        lua_pushcfunction( ls , LuaEngine::recover_nested_function<T>(arg) );
        return 1;
    });
    lua_settable(ls,-3);
}


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
            C& obj_ref = to_ref<C>(obj);
            (obj_ref.*f)();
            return 0;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C& obj_ref = to_ref<C>(obj);
            (obj_ref.*f)();
            return 0;
        };
    }
    template< typename ret_type , function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C& obj_ref = to_ref<C>(obj);
            R ret = (obj_ref.*f)();
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C& obj_ref = to_ref<C>(obj);
            R ret = (obj_ref.*f)();
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
            C& obj_ref = to_ref<C>(obj);
            (obj_ref.*f)(arg1);
            return 0;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C& obj_ref = to_ref<C>(obj);
            (obj_ref.*f)(arg1);
            return 0;
        };
    }
    template< typename ret_type , function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C& obj_ref = to_ref<C>(obj);
            R ret = (obj_ref.*f)(arg1);
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C& obj_ref = to_ref<C>(obj);
            R ret = (obj_ref.*f)(arg1);
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
            C& obj_ref = to_ref<C>(obj);
            (obj_ref.*f)(arg1,arg2);
            return 0;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<std::is_same< ret_type ,void>::value>::type >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg2      arg2    = LuaEngine::pop<T_arg2>(ls);
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C& obj_ref = to_ref<C>(obj);
            (obj_ref.*f)(arg1,arg2);
            return 0;
        };
    }
    template< typename ret_type , function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg2      arg2    = LuaEngine::pop<T_arg2>(ls);
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C& obj_ref = to_ref<C>(obj);
            R ret = (obj_ref.*f)(arg1,arg2);
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
    template< typename ret_type , const_function_type f , class = typename std::enable_if<!std::is_same< ret_type ,void>::value>::type , class=int >
    static lua_CFunction   generate_lambda( ){
        return []( lua_State* ls ) {
            T_arg2      arg2    = LuaEngine::pop<T_arg2>(ls);
            T_arg1      arg1    = LuaEngine::pop<T_arg1>(ls);
            class_type  obj     = LuaEngine::pop<class_type>(ls);
            C& obj_ref = to_ref<C>(obj);
            R ret = (obj_ref.*f)(arg1,arg2);
            LuaEngine::push<R>(ls,ret);
            return 1;
        };
    }
};


#endif