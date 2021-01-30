#include "color.h"
#include "core.h"
#include <algorithm>

template<> void LuaEngine::push( lua_State* ls , Color v ){
    void* userdata = lua_newuserdata( ls , sizeof(Color) );
    (*(Color*)userdata) = v;

    static bool metatable_initialized = false;
    if(!metatable_initialized){
        metatable_initialized = true;
        lua_pushstring(ls,"_SAUCER");
        lua_gettable(ls,LUA_GLOBALSINDEX);
        lua_pushstring(ls,"_METATABLES");
        lua_gettable(ls,-2);
        lua_remove(ls,-2);
        lua_pushstring(ls,"Color");
        // Pushing a vector3 metatable:
        lua_newtable(ls);
        
        // __index
        lua_pushstring(ls,"__index");
        lua_pushcfunction(ls, [](lua_State* ls)->int{
            Color* v = (Color*)lua_touserdata(ls,-2);
            const char* arg = lua_tostring(ls,-1);
            lua_pop(ls,2);
                    if(!strcmp(arg,"r"))   lua_pushnumber(ls, (float)(v->r)/255 );
            else    if(!strcmp(arg,"g"))   lua_pushnumber(ls, (float)(v->g)/255 );
            else    if(!strcmp(arg,"b"))   lua_pushnumber(ls, (float)(v->b)/255 );
            else    if(!strcmp(arg,"a"))   lua_pushnumber(ls, (float)(v->a)/255 );
            return 1;
        });
        lua_settable(ls,-3);

        // __newindex
        lua_pushstring(ls,"__newindex");
        lua_pushcfunction(ls, [](lua_State* ls)->int{
            Color* v = (Color*)lua_touserdata(ls,-3);
            const char* arg = lua_tostring(ls,-2);
            float new_val = lua_tonumber(ls,-1);
            new_val = std::min( 1.0f , std::max( 0.0f , new_val ));
            lua_pop(ls,3);
                    if(!strcmp(arg,"r"))    v->r = (unsigned char)( new_val * 255.0 ) ;
            else    if(!strcmp(arg,"g"))    v->g = (unsigned char)( new_val * 255.0 ) ;
            else    if(!strcmp(arg,"b"))    v->b = (unsigned char)( new_val * 255.0 ) ;
            else    if(!strcmp(arg,"a"))    v->a = (unsigned char)( new_val * 255.0 ) ;
            return 0;
        });
        lua_settable(ls,-3);
        lua_settable(ls,-3);
        // only [_METATABLES table] on stack now
        lua_pushstring(ls,"Color");
        lua_gettable(ls,-2);
        lua_remove(ls,-2);
    } 
    else {
        lua_pushstring(ls,"_SAUCER");
        lua_gettable(ls,LUA_GLOBALSINDEX);
        lua_pushstring(ls,"_METATABLES");
        lua_gettable(ls,-2);
        lua_remove(ls,-2);
        lua_pushstring(ls,"Color");
        lua_gettable(ls,-2);
        lua_remove(ls,-2);
    }

    lua_setmetatable(ls,-2);

    
}
template<> lua_CFunction    LuaEngine::create_lua_constructor<Color>( ){
    return [](lua_State* ls){
        unsigned char r = 255.0 * std::min( 1.0f , std::max( 0.0f , (float)lua_tonumber(ls,-4) ) );
        unsigned char g = 255.0 * std::min( 1.0f , std::max( 0.0f , (float)lua_tonumber(ls,-3) ) );
        unsigned char b = 255.0 * std::min( 1.0f , std::max( 0.0f , (float)lua_tonumber(ls,-2) ) );
        unsigned char a = 255.0 * std::min( 1.0f , std::max( 0.0f , (float)lua_tonumber(ls,-1) ) );
        lua_pop( ls , 3 );
        LuaEngine::push( ls , Color(  r,g,b,a) );
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
Color::Color( int r , int g , int b , int a ){
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}
    
Color   Color::operator*   (const Color  c     ) const {
    return Color( r*c.r / (65025.0f) ,
                  g*c.g / (65025.0f) ,
                  b*c.b / (65025.0f) ,
                  a*c.a / (65025.0f) );
}
void    Color::operator*=  (const Color  c     ){
    r *= (float)(c.r)/(255.0f);
    g *= (float)(c.g)/(255.0f);
    b *= (float)(c.b)/(255.0f);
    a *= (float)(c.a)/(255.0f);
}
Color::operator std::string() const{
    char buff[32];
    sprintf(buff,"(%d, %d, %d)",r,g,b);
    return std::string(buff);
}

Color::ColorFloat  Color::to_float() const{
    ColorFloat ret;
    ret.r = float(r)/255.0f;
    ret.g = float(g)/255.0f;
    ret.b = float(b)/255.0f;
    ret.a = float(a)/255.0f;
    return ret;
};
Color  Color::ColorFloat::to_color() const{
    Color ret;
    ret.r = r*255U;
    ret.g = g*255U;
    ret.b = b*255U;
    ret.a = a*255U;
    return ret;
};


void Color::bind_methods(){
    
}