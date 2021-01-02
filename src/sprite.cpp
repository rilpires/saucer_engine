#include "sprite.h"
#include "core.h"


template<> void LuaEngine::push( lua_State* ls , Sprite* obj ){
    if( obj )   *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = obj->get_saucer_id();
    else        *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = 0;
        
    lua_newtable(ls);
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls,[](lua_State* ls){
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
        lua_pushcfunction( ls , LuaEngine::recover_nested_function("Sprite",arg) );
        return 1;
    });
    lua_settable(ls,-3);
    lua_setmetatable(ls,-2);
}

std::unordered_map< SaucerId , Sprite* > Sprite::component_from_node;

Sprite::Sprite(){
    texture = nullptr;
    modulate = Color();
    self_modulate = Color();
    centralized = true;
}
Sprite::~Sprite(){
    SceneNode* node = get_node(); 
    if( node ) component_from_node.erase( node->get_saucer_id() );
}
ImageResource*  Sprite::get_texture() const{
    return texture;
}
void            Sprite::set_texture( ImageResource* tex ){
    texture = tex;
}
Color           Sprite::get_modulate() const {
    return modulate;
}
void            Sprite::set_modulate( Color new_col ){
    modulate = new_col;
}
Color           Sprite::get_self_modulate() const {
    return self_modulate;
}
void            Sprite::set_self_modulate( Color new_col ){
    self_modulate = new_col;
}
bool            Sprite::is_centralized() const {
    // const bool x = is_base<SaucerObject,SceneNode>::value;

    return centralized;
}
void            Sprite::set_centralized( bool new_val ){
    centralized = new_val;
}
void            Sprite::bind_methods(){
    
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_texture);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_texture);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_modulate);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_modulate);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_self_modulate);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_self_modulate);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,is_centralized);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_centralized);

}