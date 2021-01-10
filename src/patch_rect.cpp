#include "patch_rect.h"
#include "lua_engine.h"
#include "resources/image.h"

std::unordered_multimap<SaucerId,PatchRect*> PatchRect::component_from_node;

PatchRect::PatchRect() {
    for( int i = 0 ; i < 4 ; i++ )
        margins[i] = 5;
    texture = nullptr;
}

std::vector<RenderData> PatchRect::generate_render_data() const {
    std::vector<RenderData> ret;
    if( get_texture() ){
        Vector2 tex_size = get_texture()->get_size();
        for( int xi : {0,1,2} )
        for( int yi : {0,1,2} )
        {
            if(!draw_center && xi==1 && yi==1 ) continue;
            
            RenderData data;
            int dx,dy;
            if( xi==0 ){
                dx = 0;
                data.size_in_pixels.x = margins[LEFT_BORDER];
                data.uv_top_left.x = 0;
                data.uv_bottom_right.x = margins[LEFT_BORDER] / tex_size.x;
            } else 
            if( xi==1 ) {
                dx = margins[LEFT_BORDER];
                data.size_in_pixels.x = std::max( 0 , (int)get_rect_size().x-margins[LEFT_BORDER]-margins[RIGHT_BORDER] );
                data.uv_top_left.x = margins[LEFT_BORDER] / tex_size.x;
                data.uv_bottom_right.x = 1.0 - margins[RIGHT_BORDER] / tex_size.x;
            } else 
            if( xi==2 ) {
                dx = get_rect_size().x - margins[RIGHT_BORDER];
                data.size_in_pixels.x = margins[RIGHT_BORDER];
                data.uv_top_left.x = 1.0 - margins[RIGHT_BORDER] / tex_size.x;
                data.uv_bottom_right.x = 1;
            }

            if( yi==0 ){
                dy = 0;
                data.size_in_pixels.y = margins[TOP_BORDER];
                data.uv_top_left.y = 0;
                data.uv_bottom_right.y = margins[TOP_BORDER] / tex_size.y;
            } else 
            if( yi==1 ) {
                dy = margins[TOP_BORDER];
                data.size_in_pixels.y = std::max( 0 , (int)get_rect_size().y-margins[TOP_BORDER]-margins[BOTTOM_BORDER] );
                data.uv_top_left.y = margins[TOP_BORDER] / tex_size.y;
                data.uv_bottom_right.y = 1.0 - margins[BOTTOM_BORDER] / tex_size.y;
            } else 
            if( yi==2 ) {
                dy = get_rect_size().y - margins[BOTTOM_BORDER];
                data.size_in_pixels.y = margins[BOTTOM_BORDER];
                data.uv_top_left.y = 1.0 - margins[BOTTOM_BORDER] / tex_size.y;
                data.uv_bottom_right.y = 1;
            }

            
            data.model_transform = get_parent_global_transform() * Transform().translate( get_rect_pos() + Vector2(dx,dy) + data.size_in_pixels*0.5 );
            data.use_tree_transform = false;
            data.view_transform = false;
            data.texture_id = (texture)?( texture->get_texture_id() ):(0);
            data.shader_program = get_current_shader();

            ret.push_back(data);
        }
    }
    return ret;
}

short PatchRect::get_margin(int border) const {
    SAUCER_ASSERT( border >= 0 && border <= 4 );
    return margins[border];
}

void PatchRect::set_margin(int border, short new_val) {
    SAUCER_ASSERT( border >= 0 && border <= 4 );
    margins[border] = new_val;
}

TextureResource* PatchRect::get_texture() const {
    return texture;
}

void    PatchRect::set_texture(TextureResource* tex) {
    texture = tex;
}
void    PatchRect::set_draw_center( bool new_val){
    draw_center=new_val;
}
bool    PatchRect::get_draw_center() const{
    return draw_center;
}
void PatchRect::bind_methods() {
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_margin );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_margin );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_texture );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_texture );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_draw_center );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_draw_center );
}
