#include "vector.h"
#include "lua_engine.h"
#include <math.h>


template<> void LuaEngine::push<Vector2>( lua_State* ls , Vector2 v ){
    void* userdata = lua_newuserdata( ls , sizeof(Vector2) );
    (*(Vector2*)userdata) = v;
    // Pushing a vector2 metatable:
    lua_newtable(ls);
    
    // __index
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls, [](lua_State* ls){
        Vector2* v = (Vector2*)lua_touserdata(ls,-2);
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
                if(!strcmp(arg,"x"))   lua_pushnumber(ls,v->x);
        else    if(!strcmp(arg,"y"))   lua_pushnumber(ls,v->y);
        else    lua_pushcfunction(ls, LuaEngine::recover_nested_function("Vector2",arg) );
        return 1;
    });
    lua_settable(ls,-3);

    // __newindex
    lua_pushstring(ls,"__newindex");
    lua_pushcfunction(ls, [](lua_State* ls){
        Vector2* v = (Vector2*)lua_touserdata(ls,-3);
        const char* arg = lua_tostring(ls,-2);
        float new_val = lua_tonumber(ls,-1);
        lua_pop(ls,3);
                if(!strcmp(arg,"x"))    v->x=new_val;
        else    if(!strcmp(arg,"y"))    v->y=new_val;
        return 0;
    });
    lua_settable(ls,-3);

    
    #define PUSH_VECTOR2_METATABLE_OPERATION(index_str,operator)     \
    lua_pushstring(ls,index_str);                                    \
    lua_pushcfunction(ls, [](lua_State* ls)->int{                    \
        Vector2 v1 = *(Vector2*)lua_touserdata(ls,-2);               \
        Vector2 v2 = *(Vector2*)lua_touserdata(ls,-1);               \
        lua_pop(ls,2);                                               \
        LuaEngine::push(ls,v1 operator v2);                          \
        return 1;                                                    \
    });                                                              \
    lua_settable(ls,-3);

    #define PUSH_VECTOR2_METATABLE_OPERATION_MAYBE_SCALAR(index_str,operator) \
    lua_pushstring(ls,index_str);                       \
    lua_pushcfunction(ls, [](lua_State* ls)->int{       \
        Vector2 v1 = *(Vector2*)lua_touserdata(ls,-2);  \
        if( lua_isuserdata(ls,-1) ){                    \
            Vector2 v2 = LuaEngine::pop<Vector2>(ls);   \
            lua_pop(ls,2);                              \
            LuaEngine::push(ls,v1 operator v2);         \
        } else {                                        \
            float f = lua_tonumber(ls,-1);              \
            lua_pop(ls,2);                              \
            LuaEngine::push(ls,v1 operator f);          \
        }                                               \
        return 1;                                       \
    });                                                 \
    lua_settable(ls,-3);
    
    PUSH_VECTOR2_METATABLE_OPERATION( "__add" , + );
    PUSH_VECTOR2_METATABLE_OPERATION( "__sub" , - );
    PUSH_VECTOR2_METATABLE_OPERATION_MAYBE_SCALAR( "__mul" , * );
    PUSH_VECTOR2_METATABLE_OPERATION_MAYBE_SCALAR( "__div" , / );
    

    lua_setmetatable(ls,-2);

}

template<> lua_CFunction    LuaEngine::create_lua_constructor<Vector2>(){
    return [](lua_State* ls){
        float x = lua_tonumber(ls,-2);
        float y = lua_tonumber(ls,-1);
        lua_pop( ls , 2 );
        LuaEngine::push( ls , Vector2(x,y) );
        return 1;
    };
}


Vector2 Vector2::rotated( float rotation_degrees_cw ) const{
    float rotation_radians_cw = rotation_degrees_cw * M_PI / 180.0;
    float c = cos(rotation_radians_cw);
    float s = sin(rotation_radians_cw);
    return Vector2( x*c - y*s , x*s + y*c );
}

Vector2  Vector2::operator-  (                     ) const{ return Vector2(-x,-y);}
Vector2  Vector2::operator+  (const Vector2  v     ) const{ return Vector2(x+v.x,y+v.y); }
Vector2  Vector2::operator-  (const Vector2  v     ) const{ return Vector2(x-v.x,y-v.y); }
Vector2  Vector2::operator*  (const Vector2  v     ) const{ return Vector2(x*v.x,y*v.y); }
Vector2  Vector2::operator*  (const float    scale ) const{ return Vector2(x*scale,y*scale); }
Vector2  Vector2::operator/  (const Vector2  v     ) const{ return Vector2(x/v.x,y/v.y); }
Vector2  Vector2::operator/  (const float    div   ) const{ return Vector2(x/div,y/div); }
void     Vector2::operator+= (const Vector2  v     )      { x+=v.x;y+=v.y; }
void     Vector2::operator-= (const Vector2  v     )      { x-=v.x;y-=v.y; }
void     Vector2::operator*= (const Vector2  v     )      { x*=v.x;y*=v.y; }
void     Vector2::operator*= (const float    scale )      { x*=scale;y*=scale; }
void     Vector2::operator/= (const Vector2  v     )      { x/=v.x;y/=v.y; }
void     Vector2::operator/= (const float    div   )      { x/=div;y/=div; }
bool     Vector2::operator!= (const Vector2  v     )      { return x!=v.x || y!=v.y; }
bool     Vector2::operator== (const Vector2  v     )      { return x==v.x && y==v.y; }

void Vector2::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION(Vector2,rotated);
}


template<> void LuaEngine::push( lua_State* ls , Vector3 v ){
    void* userdata = lua_newuserdata( ls , sizeof(Vector3) );
    (*(Vector3*)userdata) = v;

    // Pushing a vector3 metatable:
    lua_newtable(ls);
    
    // __index
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        Vector3* v = (Vector3*)lua_touserdata(ls,-2);
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
                if(!strcmp(arg,"x"))   lua_pushnumber(ls,v->x);
        else    if(!strcmp(arg,"y"))   lua_pushnumber(ls,v->y);
        else    if(!strcmp(arg,"rotated"))   lua_pushcfunction(ls,nested_functions_db["Vector3"]["rotated"]);
        return 1;
    });
    lua_settable(ls,-3);

    // __newindex
    lua_pushstring(ls,"__newindex");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        Vector3* v = (Vector3*)lua_touserdata(ls,-3);
        const char* arg = lua_tostring(ls,-2);
        float new_val = lua_tonumber(ls,-1);
        lua_pop(ls,3);
                if(!strcmp(arg,"x"))    v->x=new_val;
        else    if(!strcmp(arg,"y"))    v->y=new_val;
        return 0;
    });
    lua_settable(ls,-3);

    
    #define PUSH_VECTOR3_METATABLE_OPERATION(index_str,operator)     \
    lua_pushstring(ls,index_str);                                    \
    lua_pushcfunction(ls, [](lua_State* ls)->int{                    \
        Vector3 v1 = *(Vector3*)lua_touserdata(ls,-2);               \
        Vector3 v2 = *(Vector3*)lua_touserdata(ls,-1);               \
        lua_pop(ls,2);                                               \
        LuaEngine::push(ls,v1 operator v2);                          \
        return 1;                                                    \
    });                                                              \
    lua_settable(ls,-3);

    #define PUSH_VECTOR3_METATABLE_OPERATION_SCALAR(index_str,operator) \
    lua_pushstring(ls,index_str);                                       \
    lua_pushcfunction(ls, [](lua_State* ls)->int{                       \
        Vector3 v1(*(Vector3*)lua_touserdata(ls,-2));                   \
        float f = lua_tonumber(ls,-1);                                  \
        lua_pop(ls,2);                                                  \
        LuaEngine::push(ls,v1 operator f);                              \
        return 1;                                                       \
    });                                                                 \
    lua_settable(ls,-3);
    
    PUSH_VECTOR3_METATABLE_OPERATION("__add",+);
    PUSH_VECTOR3_METATABLE_OPERATION("__sub",-);
    PUSH_VECTOR3_METATABLE_OPERATION_SCALAR("__mul",*);
    PUSH_VECTOR3_METATABLE_OPERATION_SCALAR("__div",/);
    

    lua_setmetatable(ls,-2);

}
template<> lua_CFunction    LuaEngine::create_lua_constructor<Vector3>(){
    return [](lua_State* ls){
        float r = lua_tonumber(ls,-4);
        float g = lua_tonumber(ls,-3);
        float b = lua_tonumber(ls,-2);
        float a = lua_tonumber(ls,-1);
        lua_pop( ls , 4 );
        LuaEngine::push( ls , Color(r,g,b,a) );
        return 1;
    };
}

void     Vector3::operator=  (const Vector3  v     )      { x=v.x;y=v.y;z=v.z; }
Vector3  Vector3::operator+  (const Vector3  v     ) const{ return Vector3(x+v.x,y+v.y,z+v.z); }
Vector3  Vector3::operator-  (const Vector3  v     ) const{ return Vector3(x-v.x,y-v.y,z-v.z); }
Vector3  Vector3::operator*  (const Vector3  v     ) const{ return Vector3(x*v.x,y*v.y,z*v.z); }
Vector3  Vector3::operator*  (const float    scale ) const{ return Vector3(x*scale,y*scale,z*scale); }
Vector3  Vector3::operator/  (const Vector3  v     ) const{ return Vector3(x/v.x,y/v.y,z/v.z); }
Vector3  Vector3::operator/  (const float    div   ) const{ return Vector3(x/div,y/div,z/div); }
void     Vector3::operator+= (const Vector3  v     )      { x+=v.x;y+=v.y;z+=v.z; }
void     Vector3::operator-= (const Vector3  v     )      { x-=v.x;y-=v.y;z-=v.z; }
void     Vector3::operator*= (const Vector3  v     )      { x*=v.x;y*=v.y;z*=v.z; }
void     Vector3::operator*= (const float    scale )      { x*=scale;y*=scale;z*=scale; }
void     Vector3::operator/= (const Vector3  v     )      { x/=v.x;y/=v.y;z/=v.z; }
void     Vector3::operator/= (const float    div   )      { x/=div;y/=div;z/=div; }

void Vector3::bind_methods(){
    
}