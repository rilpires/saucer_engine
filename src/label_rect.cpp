#include "label_rect.h"
#include "lua_engine.h"
#include "editor.h"

LabelRect::LabelRect(){
    text = "";
    font = nullptr;
    line_gap = 2;
    vertex_data = nullptr;
    vertex_data_count = 0;
    dirty_vertex_data = true;
    align_flags = HORIZONTAL_ALIGN_CENTER + VERTICAL_ALIGN_CENTER;
    editable = false;
    enter_is_newline = false;
    percent_visible = 1.0f;
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
        render_data.vertex_data_count = vertex_data_count * percent_visible;
        render_data.shader_program = get_current_shader();
        render_data.texture_id = font->get_texture_id();
        render_data.use_tree_transform = true;
        render_data.tex_is_alpha_mask = true;
        
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
    // float   scale       = float(font_size) / max_height;
    float   scale = 1; // Until add custom attributes for font type (ttf)
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
        vertex_data[ vertex_data_offset     ].pos = Vector3( get_offset().x + char_offset.x,          get_offset().y + char_offset.y,          0 );
        vertex_data[ vertex_data_offset + 1 ].pos = Vector3( get_offset().x + char_offset.x + size.x, get_offset().y + char_offset.y,          0 );
        vertex_data[ vertex_data_offset + 2 ].pos = Vector3( get_offset().x + char_offset.x,          get_offset().y + size.y + char_offset.y, 0 );
        vertex_data[ vertex_data_offset + 3 ].pos = Vector3( get_offset().x + char_offset.x + size.x, get_offset().y + size.y + char_offset.y, 0 );
        
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
        else {
            saucer_warn("invalid align_flags");
            final_x_offset = 0.0f;
        }
        
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
bool            LabelRect::get_enter_is_newline() const{
    return enter_is_newline;
}
void            LabelRect::set_enter_is_newline( bool new_val ){
    enter_is_newline = new_val;
}
float           LabelRect::get_percent_visible() const{
    return percent_visible;
}
void            LabelRect::set_percent_visible( float new_val){
    percent_visible = std::min( 1.0f , std::max( 0.0f , new_val ) );
}
void            LabelRect::cb_key( Input::InputEventKey& ev ){
    if( !editable ) return;
    if( ev.action == INPUT_EVENT_ACTION::PRESSED ){
        switch (ev.key_unicode)
        {
            case GLFW_KEY_ENTER:
                if( enter_is_newline ){
                    set_text( text + '\n' );
                    ev.is_solved = true;
                }
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

    REGISTER_COMPONENT_HELPERS(LabelRect,"label_rect");

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
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_line_gap );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_line_gap );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_align_flags );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_editable );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_editable );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_enter_is_newline );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_enter_is_newline );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , get_percent_visible );
    REGISTER_LUA_MEMBER_FUNCTION( LabelRect , set_percent_visible );

}

void            LabelRect::push_editor_items(){
#ifdef SAUCER_EDITOR
    
    AnchoredRect::push_editor_items();
    PROPERTY_STRING( this, text );
    PROPERTY_RESOURCE( this, font , FontResource );
    PROPERTY_INT( this, line_gap );
    PROPERTY_FLOAT_RANGE( this, percent_visible , 0 , 1 );
    PROPERTY_BOOL( this, editable ); ImGui::SameLine() ; PROPERTY_BOOL( this , enter_is_newline );
    std::map<int,bool> align_flags_map;
    for( auto i : {
        HORIZONTAL_ALIGN_LEFT,
        HORIZONTAL_ALIGN_CENTER,
        HORIZONTAL_ALIGN_RIGHT,
        VERTICAL_ALIGN_TOP,
        VERTICAL_ALIGN_CENTER,
        VERTICAL_ALIGN_BOTTOM
    }) align_flags_map[i] = align_flags & i;
    ImGui::Text("Horizontal align:");
    ImGui::Checkbox("Left",&align_flags_map[HORIZONTAL_ALIGN_LEFT]); 
    ImGui::SameLine();
    ImGui::Checkbox("hCenter",&align_flags_map[HORIZONTAL_ALIGN_CENTER]); 
    ImGui::SameLine();
    ImGui::Checkbox("Right",&align_flags_map[HORIZONTAL_ALIGN_RIGHT]);
    ImGui::Text("Vertical flag:");
    ImGui::Checkbox("Top",&align_flags_map[VERTICAL_ALIGN_TOP]); 
    ImGui::SameLine();
    ImGui::Checkbox("vCenter",&align_flags_map[VERTICAL_ALIGN_CENTER]); 
    ImGui::SameLine();
    ImGui::Checkbox("Bottom",&align_flags_map[VERTICAL_ALIGN_BOTTOM]);
    int old_align_flags = align_flags;
    align_flags = 0;
    for( auto p : align_flags_map ) if(p.second) align_flags += p.first;
    if( align_flags != old_align_flags ) dirty_vertex_data = true;
#endif
}
YamlNode        LabelRect::to_yaml_node() const {
    YamlNode ret = AnchoredRect::to_yaml_node();
    
    ret["text"] = text;
    ret["line_gap"] = line_gap;
    ret["align_flags"] = align_flags;
    ret["editable"] = editable;
    ret["enter_is_new_line"] = enter_is_newline;
    if(font)ret["font"] = font->get_path();
    
    return ret;
}
void            LabelRect::from_yaml_node( YamlNode yaml_node ){
    AnchoredRect::from_yaml_node(yaml_node);

    SET_FROM_YAML_NODE_PROPERTY(yaml_node,text);
    SET_FROM_YAML_NODE_PROPERTY(yaml_node,line_gap);
    SET_FROM_YAML_NODE_PROPERTY(yaml_node,align_flags);
    SET_FROM_YAML_NODE_PROPERTY(yaml_node,editable);
    SET_FROM_YAML_NODE_PROPERTY(yaml_node,enter_is_newline);
    SET_FROM_YAML_NODE_PROPERTY(yaml_node,percent_visible);
    
    if( yaml_node["font"].IsDefined() )
        font = ResourceManager::get_resource<FontResource>( yaml_node["font"].as<std::string>() );
}
