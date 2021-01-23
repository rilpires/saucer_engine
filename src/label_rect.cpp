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
    align_flags = HORIZONTAL_ALIGN_CENTER + VERTICAL_ALIGN_CENTER;
    editable = false;
    set_editable(true);
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
        render_data.tex_is_alpha_mask = true;
        
        render_data.model_transform = get_parent_global_transform() * Transform().translate(   get_rect_pos() );

        ret.push_back(render_data);
    }
    return ret;
}

void            LabelRect::update_vertex_data(){
    if( vertex_data ) delete[] vertex_data;
    vertex_data_count = 4*text.size();
    vertex_data = new VertexData[ vertex_data_count ];
    dirty_vertex_data = false;

    Vector2 rect_size = get_rect_size();
    int     max_height  = font->get_max_pixels_height();
    float   scale       = float(font_size) / max_height;
    char    last_char   = 0;
    int     last_line_index = 0;
    std::vector<int> lines_index;
    Vector2 pixel_advance = Vector2(0,0);
    for( size_t char_index = 0 ; char_index < text.size() ; char_index++ ){
        const char current_char = text[char_index];
        auto char_data = font->get_char_data( current_char );

        // Line breaking logic
        if( last_char==' ' && current_char!=last_char ){
            size_t next_space = text.find_first_of(' ',char_index);
            size_t next_newline = text.find_first_of('\n',char_index); 
            size_t sequence_end = std::min<size_t>(next_space,next_newline);
            sequence_end = std::min<size_t>(text.size(),sequence_end);
            size_t estimated_pixel_advance = 0;
            for( size_t i = char_index ; i < sequence_end ; i++ )
                estimated_pixel_advance += font->get_char_data(text[i]).pixels_advance * scale;
            if( pixel_advance.x + estimated_pixel_advance >= rect_size.x ){
                lines_index.push_back( last_line_index );
                pixel_advance.x = 0;
                pixel_advance.y += (max_height)*scale + line_gap;
                last_line_index = char_index;
            }
        } if( current_char == '\n' ){
            lines_index.push_back( last_line_index );
            pixel_advance.x = 0;
            pixel_advance.y += (max_height)*scale + line_gap;
            last_line_index = char_index;
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
    lines_index.push_back(last_line_index);

    float final_y_offset;
    if( align_flags & VERTICAL_ALIGN_TOP )
        final_y_offset = 0;
    if( align_flags & VERTICAL_ALIGN_CENTER )
        final_y_offset = (rect_size.y - vertex_data[vertex_data_count-1].pos.y)*0.5;
    if( align_flags & VERTICAL_ALIGN_BOTTOM )
        final_y_offset = (rect_size.y - vertex_data[vertex_data_count-1].pos.y);
    
    // Now that the text is "horizontal left aligned", transform all it's vertex now, knowing line widths
    for( size_t i = 0 ; i < lines_index.size() ; i++ ){
        size_t vertex_data_begin = lines_index[i] * 4;
        size_t vertex_data_end = (i<lines_index.size()-1) ? (lines_index[i+1] * 4) : (text.size()*4);
        int line_width = vertex_data[vertex_data_end-1].pos.x;
        float final_x_offset;

        if( align_flags & HORIZONTAL_ALIGN_LEFT )
            final_x_offset = 0;
        else 
        if( align_flags & HORIZONTAL_ALIGN_CENTER )
            final_x_offset = (rect_size.x - line_width)*0.5;
        else
        if( align_flags & HORIZONTAL_ALIGN_RIGHT )
            final_x_offset = rect_size.x - line_width;
        
        for( size_t j = vertex_data_begin ; j < vertex_data_end ; j++ )
            vertex_data[j].pos = Vector3(   vertex_data[j].pos.x + final_x_offset , 
                                            vertex_data[j].pos.y + final_y_offset , 
                                            vertex_data[j].pos.z ) ;

    }
}
std::string     LabelRect::get_text() const {
    return text;
}
void            LabelRect::set_text(std::string s) {
    if( (text.size()!=s.size()) || (text != s) ){
        text = s;
        dirty_vertex_data = true;
    }
}
FontResource*   LabelRect::get_font() const {
    return font;
}
void            LabelRect::set_font(FontResource* f) {
    if(font != f){
        font = f;
        dirty_vertex_data = true;
    }
}
int             LabelRect::get_font_size() const{
    return font_size;
}
void            LabelRect::set_font_size(int new_val){
    if( font_size != new_val ){
        font_size = new_val;
        dirty_vertex_data = true;
    }
}
int             LabelRect::get_line_gap() const{
    return line_gap;
}
void            LabelRect::set_line_gap(int new_val){
    if( line_gap != new_val ){
        line_gap = new_val;
        dirty_vertex_data = true;
    }
}
void            LabelRect::set_align_flags( int new_val ){
    if( (new_val&(HORIZONTAL_ALIGN_LEFT|HORIZONTAL_ALIGN_CENTER|HORIZONTAL_ALIGN_RIGHT)) == 0 )
        new_val |= HORIZONTAL_ALIGN_LEFT;
    if( (new_val&(VERTICAL_ALIGN_TOP|VERTICAL_ALIGN_CENTER|VERTICAL_ALIGN_BOTTOM)) == 0 )
        new_val |= VERTICAL_ALIGN_TOP;
    align_flags = new_val;
    dirty_vertex_data = true;
}
bool            LabelRect::get_editable() const{
    return editable;
}
void            LabelRect::set_editable( bool new_val ){
    if( editable != new_val ){
        editable = new_val;
        dirty_vertex_data = true;
        this->set_ignore_mouse(!new_val);
    }
}
void            LabelRect::cb_key( Input::InputEventKey& ev ){
    if( !editable ) return;
    if( ev.action == INPUT_EVENT_ACTION::PRESSED ){
        switch (ev.key_unicode)
        {
            case GLFW_KEY_ENTER:
                set_text( text + '\n' );
                ev.is_solved = true;
                break;
            
            case GLFW_KEY_BACKSPACE:
                if( text.size() > 0 )
                    set_text( text.substr(0,text.size()-1) );
                ev.is_solved = true;
                break;
            
            default: break;
        }
    }
}
void            LabelRect::cb_mouse_button( Input::InputEventMouseButton& ev ){
    UNUSED(ev);
    // Cursor aiming at a specific location
}
void            LabelRect::cb_char( Input::InputEventChar& ev ){
    if( !editable ) return;
    set_text( text + (char)ev.unicode );
    ev.is_solved = true;
}
void            LabelRect::bind_methods() {

    REGISTER_LUA_CONSTANT( LABEL_ALIGN , HORIZONTAL_ALIGN_LEFT   , HORIZONTAL_ALIGN_LEFT    );
    REGISTER_LUA_CONSTANT( LABEL_ALIGN , HORIZONTAL_ALIGN_CENTER , HORIZONTAL_ALIGN_CENTER  );
    REGISTER_LUA_CONSTANT( LABEL_ALIGN , HORIZONTAL_ALIGN_RIGHT  , HORIZONTAL_ALIGN_RIGHT   );
    REGISTER_LUA_CONSTANT( LABEL_ALIGN , VERTICAL_ALIGN_TOP      , VERTICAL_ALIGN_TOP       );
    REGISTER_LUA_CONSTANT( LABEL_ALIGN , VERTICAL_ALIGN_CENTER   , VERTICAL_ALIGN_CENTER    );
    REGISTER_LUA_CONSTANT( LABEL_ALIGN , VERTICAL_ALIGN_BOTTOM   , VERTICAL_ALIGN_BOTTOM    );

    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_text );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_text );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_font );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_font );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_font_size );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_font_size );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_line_gap );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_line_gap );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_align_flags );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_editable );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_editable );

}
YamlNode        LabelRect::to_yaml_node() const {
    YamlNode ret = AnchoredRect::to_yaml_node();
    
    ret["text"] = text;
    ret["font_size"] = font_size;
    ret["line_gap"] = line_gap;
    ret["align_flags"] = align_flags;
    ret["editable"] = editable;
    if(font)ret["font"] = font->get_path();
    
    return ret;
}
void            LabelRect::from_yaml_node( YamlNode yaml_node ){
    AnchoredRect::from_yaml_node(yaml_node);

    set_text( yaml_node["text"].as<decltype(text)>() );
    set_font_size( yaml_node["font_size"].as<decltype(font_size)>() );
    set_line_gap( yaml_node["line_gap"].as<decltype(line_gap)>() );
    set_align_flags( yaml_node["align_flags"].as<decltype(align_flags)>() );
    set_editable( yaml_node["editable"].as<decltype(editable)>() );

    if( yaml_node["font"].IsDefined() )
        font = (FontResource*)ResourceManager::get_resource( yaml_node["font"].as<std::string>() );
}
