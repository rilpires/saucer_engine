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

        SAUCER_ASSERT(margins[LEFT_BORDER]+margins[RIGHT_BORDER] <= tex_size.x );
        SAUCER_ASSERT(margins[TOP_BORDER]+margins[BOTTOM_BORDER] <= tex_size.y );
    
        Vector2 rect_size = get_rect_size();
        Vector2 effective_rect_size;
        std::vector<int> dxs , dys;
        int middle_width = rect_size.x - margins[LEFT_BORDER] - margins[RIGHT_BORDER];
        int middle_height = rect_size.y - margins[BOTTOM_BORDER] - margins[TOP_BORDER];
        int texture_middle_width = tex_size.x - margins[LEFT_BORDER] - margins[RIGHT_BORDER];
        int texture_middle_height = tex_size.y - margins[TOP_BORDER] - margins[BOTTOM_BORDER];
        effective_rect_size.x = std::max( (int)rect_size.x , margins[LEFT_BORDER] + margins[RIGHT_BORDER] );
        effective_rect_size.y = std::max( (int)rect_size.y , margins[TOP_BORDER] + margins[BOTTOM_BORDER] );
        
        dxs.push_back(0);
        for( int dx = margins[LEFT_BORDER] ; dx <= effective_rect_size.x - margins[RIGHT_BORDER] ; dx += texture_middle_width )
            dxs.push_back(dx);
        if( middle_width>0 ) dxs.push_back( rect_size.x - margins[RIGHT_BORDER] );

        dys.push_back(0);
        for( int dy = margins[TOP_BORDER] ; dy <= effective_rect_size.y - margins[BOTTOM_BORDER] ; dy += texture_middle_height )
            dys.push_back(dy);
        if( middle_height>0 ) dys.push_back( rect_size.y - margins[BOTTOM_BORDER] );


        for( int xi = 0 ; xi < dxs.size() ; xi++ )
        for( int yi = 0 ; yi < dys.size() ; yi++ )
        {
            RenderData data;
            int dx = dxs[xi] , dy = dys[yi];
            if( xi==0 ){
                data.size_in_pixels.x = margins[LEFT_BORDER];
                data.uv_top_left.x = 0;
                data.uv_bottom_right.x = margins[LEFT_BORDER] / tex_size.x;
            } else 
            if( xi>=1 && xi< dxs.size()-1 ) {
                data.size_in_pixels.x = std::min( texture_middle_width , (int)rect_size.x-margins[RIGHT_BORDER]-dx );
                data.uv_top_left.x = margins[LEFT_BORDER] / tex_size.x;
                data.uv_bottom_right.x = data.uv_top_left.x + data.size_in_pixels.x/tex_size.x;
            } else 
            if( xi==dxs.size()-1 ) {
                data.size_in_pixels.x = margins[RIGHT_BORDER];
                data.uv_top_left.x = 1.0 - margins[RIGHT_BORDER] / tex_size.x;
                data.uv_bottom_right.x = 1;
            }

            if( yi==0 ){
                data.size_in_pixels.y = margins[TOP_BORDER];
                data.uv_top_left.y = 0;
                data.uv_bottom_right.y = margins[TOP_BORDER] / tex_size.y;
            } else 
            if( yi>=1 && yi< dys.size()-1 ) {
                data.size_in_pixels.y = std::min( texture_middle_height , (int)rect_size.y-margins[BOTTOM_BORDER]-dy );
                data.uv_top_left.y = margins[TOP_BORDER] / tex_size.y;
                data.uv_bottom_right.y = data.uv_top_left.y + data.size_in_pixels.y/tex_size.y;
            } else 
            if( yi==dys.size()-1 ) {
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

void PatchRect::set_texture(TextureResource* tex) {
    texture = tex;
}

void PatchRect::bind_methods() {
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_margin );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_margin );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_texture );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_texture );
}
