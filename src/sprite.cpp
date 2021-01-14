#include "sprite.h"
#include "resources/image.h"
#include "lua_engine.h"
#include <algorithm>

std::unordered_multimap< SaucerId , Sprite* > Sprite::component_from_node;

Sprite::Sprite() {
    texture = nullptr;
    centralized = false;
    h_frames = 1;
    v_frames = 1;
    frame_index = 0;
    vertex_data = new VertexData[4];
    vertex_data_count = 4;
}
Sprite::~Sprite(){
    delete[] vertex_data;
}

std::vector<RenderData>  Sprite::generate_render_data(){
    std::vector<RenderData> ret;
    if( texture ){
        RenderData render_data;
        
        Vector2 region_top_left_uv = region_top_left / texture->get_size();
        Vector2 region_bottom_right_uv = region_bottom_right / texture->get_size();
        Vector2 uv_size = (region_bottom_right_uv-region_top_left_uv)/Vector2( h_frames , v_frames );
        Vector2 top_left_uv = region_top_left_uv + uv_size * Vector2( frame_index%h_frames , frame_index/h_frames );
        Vector2 bottom_right_uv = top_left_uv + uv_size;
        Vector2 size = texture->get_size() * uv_size ;

        vertex_data[0].pos  = Vector3(0,0,0);
        vertex_data[0].uv   = top_left_uv;
        
        vertex_data[1].pos  = Vector3(size.x,0,0);
        vertex_data[1].uv   = Vector2(bottom_right_uv.x , top_left_uv.y);
        
        vertex_data[2].pos  = Vector3(0,size.y,0);
        vertex_data[2].uv   = Vector2(top_left_uv.x , bottom_right_uv.y);

        vertex_data[3].pos  = Vector3(size.x,size.y,0);
        vertex_data[3].uv   = bottom_right_uv;
        
        if( centralized ){ // centered
            vertex_data[0].pos -= Vector3( size.x*0.5 , size.y*0.5 , 0 );
            vertex_data[1].pos -= Vector3( size.x*0.5 , size.y*0.5 , 0 );
            vertex_data[2].pos -= Vector3( size.x*0.5 , size.y*0.5 , 0 );
            vertex_data[3].pos -= Vector3( size.x*0.5 , size.y*0.5 , 0 );
        }

        render_data.vertex_data = vertex_data;
        render_data.vertex_data_count = vertex_data_count;
        render_data.texture_id  = texture->get_texture_id();
        render_data.shader_program = get_current_shader();
        render_data.use_tree_transform = true;
        render_data.use_view_transform = true;

        ret.push_back(render_data);
    }
    return ret;
}
TextureResource*  Sprite::get_texture() const{
    return texture;
}
void            Sprite::set_texture( TextureResource* tex ){
    texture = tex;
    if( texture ){
        region_top_left = Vector2(0,0);
        region_bottom_right = texture->get_size();
    }
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
Vector2         Sprite::get_region_top_left() const{
    return region_top_left;
}
Vector2         Sprite::get_region_bottom_right() const{
    return region_bottom_right;
}
void            Sprite::set_region_top_left(Vector2 new_val){
    region_top_left = new_val;
}
void            Sprite::set_region_bottom_right(Vector2 new_val){
    region_bottom_right = new_val;
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
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_region_top_left);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_region_bottom_right);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_region_top_left);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_region_bottom_right);

}