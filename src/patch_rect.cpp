#include "patch_rect.h"
#include "lua_engine.h"
#include "resources/image.h"

std::unordered_multimap<SaucerId,PatchRect*> PatchRect::component_from_node;

PatchRect::PatchRect() {
    for( int i = 0 ; i < 4 ; i++ )
        margins[i] = 4;
    texture = nullptr;
}

std::vector<RenderData> PatchRect::generate_render_data() const {
    std::vector<RenderData> ret;
    Vector2 global_rect_pos = get_global_rect_pos();
    for( int x : {0,1,2} )
    for( int y : {0,1,2} )
    {
        RenderData data;
        int dx,dy;
        
        if( x==0 ){
            dx = 0;
            data.size_in_pixels.x = margins[LEFT_BORDER];
        } else 
        if( x== 1 ) {
            dx = margins[LEFT_BORDER];
            data.size_in_pixels.x = get_rect_size().x - margins[LEFT_BORDER]- margins[RIGHT_BORDER];
        } else 
        if( x== 2 ) {
            dx = get_rect_size().x - margins[RIGHT_BORDER];
            data.size_in_pixels.x = margins[RIGHT_BORDER];
        }
        
        if( y==0 ){
            dy = 0;
            data.size_in_pixels.y = margins[TOP_BORDER];
        } else 
        if( y== 1 ) {
            dy = margins[TOP_BORDER];
            data.size_in_pixels.y = get_rect_size().y - margins[TOP_BORDER]- margins[BOTTOM_BORDER];
        } else 
        if( y== 2 ) {
            dy = get_rect_size().y - margins[BOTTOM_BORDER];
            data.size_in_pixels.y = margins[BOTTOM_BORDER];
        }

        data.model_transform = Transform().translate( global_rect_pos + (Vector2(dx,dy)+data.size_in_pixels*0.5)*Vector2(1,-1) /*Flipped y for controls*/ );
        data.use_tree_transform = false;
        data.view_transform = false;
        data.texture_id = (texture)?( texture->get_texture_id() ):(0);
        data.shader_program = get_current_shader();
        data.uv_top_left = Vector2(  );
        data.uv_bottom_right = Vector2( 1,1);
        ret.push_back(data);
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

void PatchRect::set_texture(TextureResource* tex) {
    texture = tex;
}

void PatchRect::bind_methods() {
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_margin );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_margin );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_texture );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_texture );
}
