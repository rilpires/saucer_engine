#include "label_rect.h"
#include "lua_engine.h"

std::unordered_multimap<SaucerId,LabelRect*> LabelRect::component_from_node;

LabelRect::LabelRect(){
    text = "";
    font = nullptr;
    font_size = 12;
    line_gap = 2;
}

std::vector<RenderData>  LabelRect::generate_render_data() const {
    std::vector<RenderData> ret;
    if( font ) {
        int max_height = font->get_max_pixels_height();
        float scale = float(font_size) / max_height;
        char last_char = 0;
        Vector2 pixel_advance = Vector2(0,0);
        for( size_t char_index = 0 ; char_index < text.size() ; char_index++ ){
            RenderData data;
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
            auto char_data = font->get_char_data( text[char_index] );
            data.shader_program = get_current_shader();
            data.size_in_pixels = char_data.pixel_size * scale;
            Vector2 char_center_translation = pixel_advance 
                                            + Vector2(0,max_height) * scale 
                                            + char_data.bearing*Vector2(1,-1) * scale 
                                            + data.size_in_pixels* 0.5;
            data.model_transform = get_parent_global_transform() * Transform().translate(   get_rect_pos() + char_center_translation );
            data.texture_id = 3;
            data.use_tree_transform = false;
            data.uv_top_left = char_data.top_left_uv;
            data.uv_bottom_right = char_data.bottom_right_uv;
            data.view_transform = false;
            data.tex_is_alpha_mask = true;
            pixel_advance.x += char_data.pixels_advance * scale;
            last_char = text[char_index];
            ret.push_back(data);
        }
    }
    return ret;
}

std::string LabelRect::get_text() const {
    return text;
}

void LabelRect::set_text(std::string s) {
    text = s;
}

FontResource* LabelRect::get_font() const {
    return font;
}

void LabelRect::set_font(FontResource* f) {
    font = f;
}
int    LabelRect::get_line_gap() const{
    return line_gap;
}
void   LabelRect::set_line_gap(int new_val){
    line_gap = new_val;
}

void LabelRect::bind_methods() {

    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_text );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_text );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_font );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_font );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_line_gap );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_line_gap );

}
