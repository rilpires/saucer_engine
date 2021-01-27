#include "sprite.h"
#include "core.h"
#include <algorithm>

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
        
        Vector2 region_top_left_uv = region.top_left / texture->get_size();
        Vector2 region_bottom_right_uv = region.bottom_right / texture->get_size();
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

        ret.push_back(render_data);
    }
    return ret;
}
TextureResource*  Sprite::get_texture() const{
    return texture;
}
void            Sprite::set_texture( TextureResource* tex ){
    TextureResource* old_texture = texture;
    texture = tex;
    if( texture ){
        if( region==Rect(0,0,0,0) || !old_texture || (old_texture && region.get_size()==old_texture->get_size() ) ) 
            region = Rect( Vector2(0,0) , texture->get_size() );
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
bool            Sprite::get_centralized() const {
    return centralized;
}
void            Sprite::set_centralized( bool new_val ){
    centralized = new_val;
}
Rect            Sprite::get_region() const{
    return region;
}
void            Sprite::set_region(Rect new_val){
    region = new_val;
}
void            Sprite::bind_methods(){
    
    
    REGISTER_COMPONENT_HELPERS(Sprite,"sprite");

    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_texture);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_texture);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_h_frames);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_h_frames);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_v_frames);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_v_frames);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_frame_index);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_frame_index);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_centralized);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_centralized);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,get_region);
    REGISTER_LUA_MEMBER_FUNCTION(Sprite,set_region);

}
void            Sprite::push_editor_items(){
    PROPERTY_RESOURCE(this,texture,TextureResource);
    PROPERTY_INT(this,h_frames);
    PROPERTY_INT(this,v_frames);
    PROPERTY_INT(this,frame_index);
    PROPERTY_BOOL(this,centralized);
    PROPERTY_RECT(this,region);
}
YamlNode        Sprite::to_yaml_node() const {
    YamlNode ret;
    if(texture) ret["texture"] = texture->get_path();
    ret["h_frames"] = h_frames;
    ret["v_frames"] = v_frames;
    ret["frame_index"] = frame_index;
    ret["centralized"] = centralized;
    ret["region"] = region;
    return ret;
}
void            Sprite::from_yaml_node( YamlNode yaml_node ){
    if( yaml_node["texture"].IsDefined() )
        set_texture(ResourceManager::get_resource<TextureResource>(yaml_node["texture"].as<std::string>()));
    SET_FROM_YAML_NODE_PROPERTY(yaml_node,h_frames);
    SET_FROM_YAML_NODE_PROPERTY(yaml_node,v_frames);
    SET_FROM_YAML_NODE_PROPERTY(yaml_node,frame_index);
    SET_FROM_YAML_NODE_PROPERTY(yaml_node,centralized);
    SET_FROM_YAML_NODE_PROPERTY(yaml_node,region);
}