#include "color.h"
#include "core.h"

template<> void LuaEngine::push( lua_State* ls , Color v ){
    void* userdata = lua_newuserdata( ls , sizeof(Color) );
    (*(Color*)userdata) = v;

    // Pushing a vector3 metatable:
    lua_newtable(ls);
    
    // __index
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        Color* v = (Color*)lua_touserdata(ls,-2);
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
                if(!strcmp(arg,"r"))   lua_pushnumber(ls,v->r);
        else    if(!strcmp(arg,"g"))   lua_pushnumber(ls,v->g);
        else    if(!strcmp(arg,"b"))   lua_pushnumber(ls,v->b);
        else    if(!strcmp(arg,"a"))   lua_pushnumber(ls,v->a);
        else    if(!strcmp(arg,"rotated"))   lua_pushcfunction(ls,nested_functions_db["Color"]["rotated"]);
        return 1;
    });
    lua_settable(ls,-3);

    // __newindex
    lua_pushstring(ls,"__newindex");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        Color* v = (Color*)lua_touserdata(ls,-3);
        const char* arg = lua_tostring(ls,-2);
        float new_val = lua_tonumber(ls,-1);
        lua_pop(ls,3);
                if(!strcmp(arg,"r"))    v->r=new_val;
        else    if(!strcmp(arg,"g"))    v->g=new_val;
        else    if(!strcmp(arg,"b"))    v->b=new_val;
        else    if(!strcmp(arg,"a"))    v->a=new_val;
        return 0;
    });
    lua_settable(ls,-3);
    
    lua_setmetatable(ls,-2);
}
LUAENGINE_POP_USERDATA_AS_VALUE(Color)
template<> lua_CFunction    LuaEngine::create_lua_constructor<Color>( lua_State* ls ){
    return [](lua_State* ls){
        float x = lua_tonumber(ls,-3);
        float y = lua_tonumber(ls,-2);
        float z = lua_tonumber(ls,-1);
        lua_pop( ls , 3 );
        LuaEngine::push( ls , Vector3(x,y,z) );
        return 1;
    };
}

Color::Color( float r , float g , float b , float a ){
    this->r = r * 255U;
    this->g = g * 255U;
    this->b = b * 255U;
    this->a = a * 255U;
}
Color::Color( unsigned char r , unsigned char g , unsigned char b , unsigned char a ){
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}
Color::Color( int color ){
    this->r = (color >> 24) | 255;
    this->g = (color >> 16) | 255;
    this->b = (color >> 8)  | 255;
    this->a = (color >> 0)  | 255;
}
Color::operator std::string() const{
    char buff[32];
    sprintf(buff,"(%d, %d, %d)",r,g,b);
    return std::string(buff);
}


void Color::bind_methods(){
    
}