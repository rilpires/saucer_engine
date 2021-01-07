#include "sprite.h"
#include "resources/image.h"
#include "lua_engine.h"
#include <algorithm>

std::unordered_multimap< SaucerId , Sprite* > Sprite::component_from_node;

Sprite::Sprite() {
    texture = nullptr;
    centralized = true;
    h_frames = 1;
    v_frames = 1;
    frame_index = 0;
}
Sprite::~Sprite(){
    SceneNode* node = get_node(); 
    if( node ) component_from_node.erase( node->get_saucer_id() );
}

RenderObject::RenderData      Sprite::get_render_data() const {
    if( texture ){
        RenderData ret;
        ret.texture_id      = texture->get_texture_id();
        ret.uv_top_left     = Vector2(  (1.0/h_frames)*(frame_index%h_frames) ,   (1.0/v_frames)*(frame_index/h_frames) );
        ret.uv_bottom_right = ret.uv_top_left + Vector2( 1.0/h_frames , 1.0/v_frames ) ;
        return ret;
    } else return RenderObject::get_render_data();
}
ImageResource*  Sprite::get_texture() const{
    return texture;
}
void            Sprite::set_texture( ImageResource* tex ){
    texture = tex;
}
short           Sprite::get_h_frames() const{
    return h_frames;
}
void            Sprite::set_h_frames( short new_val ){
    h_frames = std::max((short)1,new_val);
}
short           Sprite::get_v_frames() const{
    return v_frames;
}
void            Sprite::set_v_frames( short new_val ){
    v_frames = std::max((short)1,new_val); 
}
short           Sprite::get_frame_index() const{
    return frame_index;
}
void            Sprite::set_frame_index( short new_val ){
    frame_index = new_val % ( h_frames * v_frames );
}
bool            Sprite::is_centralized() const {
    return centralized;
}
void            Sprite::set_centralized( bool new_val ){
    centralized = new_val;
}
void            Sprite::bind_methods(){
    
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_texture);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_texture);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_h_frames);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_h_frames);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_v_frames);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_v_frames);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_frame_index);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_frame_index);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,is_centralized);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_centralized);

}