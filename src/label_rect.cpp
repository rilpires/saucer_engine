#include "label_rect.h"
#include "lua_engine.h"

std::unordered_multimap<SaucerId,LabelRect*> LabelRect::component_from_node;

LabelRect::LabelRect(){
    text = "";
    font = nullptr;
    font_size = 12;
    line_gap = 2;
    vertex_data = nullptr;
    vertex_data_count = 0;
    dirty_vertex_data = true;
}
LabelRect::~LabelRect(){
    if(vertex_data)delete[] vertex_data;
}

std::vector<RenderData>  LabelRect::generate_render_data(){
    std::vector<RenderData> ret;
    if( font && text.size() ) {
        RenderData render_data;

        if( dirty_vertex_data ) update_vertex_data();

        render_data.vertex_data = vertex_data;
        render_data.vertex_data_count = vertex_data_count;
        render_data.shader_program = get_current_shader();
        render_data.texture_id = font->get_texture_id();
        render_data.use_tree_transform = false;
        render_data.use_view_transform = false;
        render_data.tex_is_alpha_mask = true;
        
        render_data.model_transform = get_parent_global_transform() * Transform().translate(   get_rect_pos() );

        ret.push_back(render_data);
    }
    return ret;
}

void    LabelRect::update_vertex_data(){
    if( vertex_data ) delete[] vertex_data;
    vertex_data_count = 4*text.size();
    vertex_data = new VertexData[ vertex_data_count ];
    dirty_vertex_data = false;

    int     max_height  = font->get_max_pixels_height();
    float   scale       = float(font_size) / max_height;
    char    last_char   = 0;
    Vector2 pixel_advance = Vector2(0,0);
    for( size_t char_index = 0 ; char_index < text.size() ; char_index++ ){
        
        auto char_data = font->get_char_data( text[char_index] );

        // Line breaking logic
        if( text[char_index]!=last_char && last_char == ' '  ){
            size_t next_space = text.find_first_of(' ',char_index);
            if( next_space == std::string::npos )
                next_space = text.size();
            if( next_space != std::string::npos ){
                size_t estimated_pixel_advance = 0;
                for( size_t i = char_index ; i < next_space ; i++ )
                    estimated_pixel_advance += font->get_char_data(text[i]).pixels_advance * scale;
                if( pixel_advance.x + estimated_pixel_advance >= get_rect_size().x ){
                    pixel_advance.x = 0;
                    pixel_advance.y += (max_height)*scale + line_gap;
                }
            }
        }

        Vector2 size = char_data.pixel_size * scale;
        Vector2 char_offset = pixel_advance 
                              + Vector2(0,max_height) * scale 
                              + char_data.bearing*Vector2(1,-1) * scale;

        unsigned short vertex_data_offset = 4*char_index;
        vertex_data[ vertex_data_offset     ].pos = Vector3( char_offset.x,             char_offset.y,             0);
        vertex_data[ vertex_data_offset + 1 ].pos = Vector3( char_offset.x + size.x,    char_offset.y,             0);
        vertex_data[ vertex_data_offset + 2 ].pos = Vector3( char_offset.x,             size.y + char_offset.y,    0);
        vertex_data[ vertex_data_offset + 3 ].pos = Vector3( char_offset.x + size.x,    size.y + char_offset.y,    0);
        
        vertex_data[ vertex_data_offset     ].uv = char_data.top_left_uv;
        vertex_data[ vertex_data_offset + 1 ].uv = Vector2(char_data.bottom_right_uv.x, char_data.top_left_uv.y);
        vertex_data[ vertex_data_offset + 2 ].uv = Vector2(char_data.top_left_uv.x,     char_data.bottom_right_uv.y);
        vertex_data[ vertex_data_offset + 3 ].uv = char_data.bottom_right_uv;

        pixel_advance.x += char_data.pixels_advance * scale;
        last_char = text[char_index];
    }
}

std::string LabelRect::get_text() const {
    return text;
}

void LabelRect::set_text(std::string s) {
    text = s;
    dirty_vertex_data = true;
}

FontResource* LabelRect::get_font() const {
    return font;
}

void LabelRect::set_font(FontResource* f) {
    font = f;
    dirty_vertex_data = true;
}
int     LabelRect::get_font_size() const{
    return font_size;
}
void    LabelRect::set_font_size(int new_val){
    font_size = new_val;
}
int    LabelRect::get_line_gap() const{
    return line_gap;
}
void   LabelRect::set_line_gap(int new_val){
    line_gap = new_val;
    dirty_vertex_data = true;
}

void LabelRect::bind_methods() {

    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_text );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_text );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_font );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_font );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_font_size );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_font_size );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_line_gap );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_line_gap );

}
