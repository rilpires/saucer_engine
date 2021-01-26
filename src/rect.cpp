#include "rect.h"
#include "lua_engine.h"

#include <array>
#include <algorithm>

template<> void LuaEngine::push<Rect>( lua_State* ls , Rect r ){
    void* userdata = lua_newuserdata( ls , sizeof(Rect) );
    (*(Rect*)userdata) = r;

    // Pushing a Rect metatable:
    lua_newtable(ls);
    
    // __index
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls, [](lua_State* ls){
        Rect* r = (Rect*)lua_touserdata(ls,-2);
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
                if(!strcmp(arg,"x1"))   lua_pushnumber(ls,r->top_left.x);
        else    if(!strcmp(arg,"y1"))   lua_pushnumber(ls,r->top_left.y);
        else    if(!strcmp(arg,"x2"))   lua_pushnumber(ls,r->bottom_right.x);
        else    if(!strcmp(arg,"y2"))   lua_pushnumber(ls,r->bottom_right.y);
        else    lua_pushcfunction(ls, LuaEngine::recover_nested_function("Rect",arg) );
        return 1;
    });
    lua_settable(ls,-3);

    // __newindex
    lua_pushstring(ls,"__newindex");
    lua_pushcfunction(ls, [](lua_State* ls){
        Rect* r = (Rect*)lua_touserdata(ls,-3);
        const char* arg = lua_tostring(ls,-2);
        float new_val = lua_tonumber(ls,-1);
        lua_pop(ls,3);
                if(!strcmp(arg,"x1"))    r->top_left.x=new_val;
        else    if(!strcmp(arg,"y1"))    r->top_left.y=new_val;
        else    if(!strcmp(arg,"x2"))    r->bottom_right.x=new_val;
        else    if(!strcmp(arg,"y2"))    r->bottom_right.y=new_val;
        return 0;
    });
    lua_settable(ls,-3);

    lua_setmetatable(ls,-2);

}

template<> lua_CFunction    LuaEngine::create_lua_constructor<Rect>(){
    return [](lua_State* ls){
        float x1 = lua_tonumber(ls,-4);
        float y1 = lua_tonumber(ls,-3);
        float x2 = lua_tonumber(ls,-2);
        float y2 = lua_tonumber(ls,-1);
        lua_pop( ls , 4 );
        LuaEngine::push( ls , Rect(x1,y1,x2,y2) );
        return 1;
    };
}


Rect::Rect( Vector2 p_top_left , Vector2 p_bottom_right ){
    top_left = p_top_left;
    bottom_right = p_bottom_right;
}
Rect::Rect( float p_x1 , float p_y1 , float p_x2 , float p_y2 ){
    top_left.x = p_x1; bottom_right.x = p_x2;  
    top_left.y = p_y1; bottom_right.y = p_y2;
}
Rect        Rect::rect_intersection(Rect r) const{
    if( r.top_left.x >= bottom_right.x || top_left.x >= r.bottom_right.x || r.top_left.y >= bottom_right.y || top_left.y >= r.bottom_right.y ){
        return Rect(0,0,0,0);
    } else {
        std::array<float,4> xs = { top_left.x , bottom_right.x , r.top_left.x , r.bottom_right.x };
        std::array<float,4> ys = { top_left.y , bottom_right.y , r.top_left.y , r.bottom_right.y };
        std::sort( xs.begin() , xs.end() );
        std::sort( ys.begin() , ys.end() );
        return Rect( xs[1] , ys[1] , xs[2] , ys[2] );
    }
}
Rect        Rect::rect_union(Rect r) const{
    return Rect(
        std::min( top_left.x , r.bottom_right.x ),
        std::min( top_left.y , r.bottom_right.y ),
        std::max( top_left.x , r.bottom_right.x ),
        std::max( top_left.y , r.bottom_right.y )
    );
}
Vector2     Rect::get_size() const{
    return bottom_right - top_left;
}
float       Rect::get_area() const{
    Vector2 size = get_size();
    return size.x * size.y;
}
float       Rect::get_width() const{
    return bottom_right.x - top_left.x;
}
float       Rect::get_height() const{
    return bottom_right.x - top_left.y;
}
bool        Rect::is_point_inside( Vector2 p ) const{
    return (p.x >= top_left.x) && (p.x <= bottom_right.x) && (p.y >= top_left.y) && (p.y <= bottom_right.y);
}

bool Rect::operator==( const Rect& r ) const{
    return (top_left==r.top_left) && (bottom_right == r.bottom_right);
}
Rect::operator std::string() const{
    char buff[63];
    sprintf(buff , "Rect( %.2f , %.2f , %.2f , %.2f )" , top_left.x , top_left.y , bottom_right.x , bottom_right.y );
    return std::string(buff);
}
void Rect::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION(Rect,rect_intersection);
    REGISTER_LUA_MEMBER_FUNCTION(Rect,rect_union);
    REGISTER_LUA_MEMBER_FUNCTION(Rect,get_area);
    REGISTER_LUA_MEMBER_FUNCTION(Rect,get_width);
    REGISTER_LUA_MEMBER_FUNCTION(Rect,get_height);
    REGISTER_LUA_MEMBER_FUNCTION(Rect,is_point_inside);
}
