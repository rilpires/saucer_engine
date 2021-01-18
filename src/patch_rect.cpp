#include "patch_rect.h"
#include "lua_engine.h"
#include "resources/image.h"

std::unordered_multimap<SaucerId,PatchRect*> PatchRect::component_from_node;

PatchRect::PatchRect() {
    for( int i = 0 ; i < 4 ; i++ )
        margins[i] = 5;
    texture = nullptr;
    vertex_data = new VertexData[36];
    vertex_data_count = 36;
    draw_center = true;
}
PatchRect::~PatchRect() {
    delete[] vertex_data;    
}

std::vector<RenderData> PatchRect::generate_render_data() {
    std::vector<RenderData> ret;
    if( texture ){
        RenderData render_data;
        Vector2 tex_size = texture->get_size();
        
        for( int xi : {0,1,2} ) for( int yi : {0,1,2} ) {
            if(!draw_center && xi==1 && yi==1 ) continue;
            
            Vector2 top_left_uv;
            Vector2 bottom_right_uv;
            Vector2 size;

            int dx,dy;
            if( xi==0 ){
                dx = 0;
                size.x = margins[LEFT_BORDER];
                top_left_uv.x = top_left_region.x / tex_size.x;
                bottom_right_uv.x = top_left_region.x / tex_size.x + margins[LEFT_BORDER]/tex_size.x;
            } else 
            if( xi==1 ) {
                dx = margins[LEFT_BORDER];
                size.x = std::max( 0 , (int)get_rect_size().x-margins[LEFT_BORDER]-margins[RIGHT_BORDER] );
                top_left_uv.x = top_left_region.x / tex_size.x + margins[LEFT_BORDER]/tex_size.x;
                bottom_right_uv.x = bottom_right_region.x / tex_size.x - margins[RIGHT_BORDER]/tex_size.x;
            } else 
            if( xi==2 ) {
                dx = get_rect_size().x - margins[RIGHT_BORDER];
                size.x = margins[RIGHT_BORDER];
                top_left_uv.x = bottom_right_region.x / tex_size.x - margins[RIGHT_BORDER]/tex_size.x;
                bottom_right_uv.x = bottom_right_region.x / tex_size.x;
            }

            if( yi==0 ){
                dy = 0;
                size.y = margins[TOP_BORDER];
                top_left_uv.y = top_left_region.y / tex_size.y;
                bottom_right_uv.y = top_left_region.y / tex_size.y + margins[TOP_BORDER]/tex_size.y;
            } else 
            if( yi==1 ) {
                dy = margins[TOP_BORDER];
                size.y = std::max( 0 , (int)get_rect_size().y-margins[TOP_BORDER]-margins[BOTTOM_BORDER] );
                top_left_uv.y = top_left_region.y / tex_size.y + margins[TOP_BORDER]/tex_size.y;
                bottom_right_uv.y = bottom_right_region.y / tex_size.y - margins[BOTTOM_BORDER]/tex_size.y;
            } else 
            if( yi==2 ) {
                dy = get_rect_size().y - margins[BOTTOM_BORDER];
                size.y = margins[BOTTOM_BORDER];
                top_left_uv.y = bottom_right_region.y / tex_size.y - margins[BOTTOM_BORDER]/tex_size.y;
                bottom_right_uv.y = bottom_right_region.y / tex_size.y;
            }

            unsigned short vertex_data_offset = 4*(xi + yi*3);
            vertex_data[ vertex_data_offset     ].pos = Vector3(dx,             dy,             0);
            vertex_data[ vertex_data_offset + 1 ].pos = Vector3(dx + size.x,    dy,             0);
            vertex_data[ vertex_data_offset + 2 ].pos = Vector3(dx,             size.y + dy,    0);
            vertex_data[ vertex_data_offset + 3 ].pos = Vector3(dx + size.x,    size.y + dy,    0);
            
            vertex_data[ vertex_data_offset     ].uv = top_left_uv;
            vertex_data[ vertex_data_offset + 1 ].uv = Vector2(bottom_right_uv.x,top_left_uv.y);
            vertex_data[ vertex_data_offset + 2 ].uv = Vector2(top_left_uv.x,bottom_right_uv.y);
            vertex_data[ vertex_data_offset + 3 ].uv = bottom_right_uv;

        }
        render_data.vertex_data = vertex_data;
        render_data.vertex_data_count = vertex_data_count;
        render_data.use_tree_transform = false;
        render_data.use_view_transform = !get_starts_on_viewport();
        render_data.model_transform = get_parent_global_transform() * Transform().translate( get_rect_pos() );
        render_data.texture_id = texture->get_texture_id();
        render_data.shader_program = get_current_shader();

        ret.push_back(render_data);
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
    if( texture ){
        top_left_region = Vector2(0,0);
        bottom_right_region = texture->get_size();
    }
}
void    PatchRect::set_draw_center( bool new_val){
    draw_center=new_val;
}
bool    PatchRect::get_draw_center() const{
    return draw_center;
}
Vector2     PatchRect::get_top_left_region() const{
    return top_left_region;
}
void        PatchRect::set_top_left_region(Vector2 new_val){
    top_left_region = new_val;
}
Vector2     PatchRect::get_bottom_right_region() const{
    return bottom_right_region;
}
void        PatchRect::set_bottom_right_region(Vector2 new_val){
    bottom_right_region = new_val;
}
void PatchRect::bind_methods() {
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_margin );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_margin );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_texture );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_texture );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_draw_center );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_draw_center );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_top_left_region );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_top_left_region );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , get_bottom_right_region );
    REGISTER_LUA_MEMBER_FUNCTION( PatchRect , set_bottom_right_region );


}
