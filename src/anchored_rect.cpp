#include "anchored_rect.h"
#include "core.h"

AnchoredRect::AnchoredRect() {
    anchored_borders[0] = 0;
    anchored_borders[1] = 0;
    anchored_borders[2] = 0;
    anchored_borders[3] = 0;
    rect_size = Vector2(16,16);
    starts_on_viewport = false;
    offset = Vector2(0,0);
    centered = false;
    ignore_mouse = true;
}
AnchoredRect::~AnchoredRect() {
    std::vector<Scene*> scenes_to_check;
    scenes_to_check.push_back( Engine::get_current_scene() );
    if( attached_node && attached_node->get_scene() ) 
        scenes_to_check.push_back( attached_node->get_scene() );
    for( Scene* scene : scenes_to_check){
        if( scene->get_current_hovered_anchored_rect() == this )
            scene->set_current_hovered_anchored_rect(nullptr);
        if( scene->get_current_focused_anchored_rect() == this )
            scene->set_current_focused_anchored_rect(nullptr);
    }
}
std::vector<RenderData>  AnchoredRect::generate_render_data(){
    std::vector<RenderData> ret;
    return ret;
}
bool            AnchoredRect::is_border_anchored( int border , int parent_border ) const {
    return (anchored_borders[border] & (1<<parent_border));
}
void            AnchoredRect::set_anchored_border( int border , int parent_border , bool new_val ){
    if (new_val)    anchored_borders[border] |=  (1<<parent_border);
    else            anchored_borders[border] &= ~(1<<parent_border);
}
Vector2         AnchoredRect::get_rect_size() const {
    return rect_size;
}
void            AnchoredRect::set_rect_size(Vector2 new_val) {
    new_val.x = std::max(0.0f,new_val.x);
    new_val.y = std::max(0.0f,new_val.y);        
    grow(RIGHT_BORDER,new_val.x - rect_size.x);
    grow(BOTTOM_BORDER,new_val.y - rect_size.y);
}
bool            AnchoredRect::get_starts_on_viewport() const {
    return starts_on_viewport;
}
void            AnchoredRect::set_starts_on_viewport(bool new_val) {
    starts_on_viewport = new_val;
}
bool            AnchoredRect::get_ignore_mouse() const{
    return ignore_mouse;
}
void            AnchoredRect::set_ignore_mouse(bool new_val){
    ignore_mouse = new_val;
}
void            AnchoredRect::grow(int border, float amount , bool updated_position ) {
    if( amount == 0 ) return;
    std::string name = attached_node->get_name();
    switch (border)
    {
        case LEFT_BORDER:
            amount = std::max(amount,-rect_size.x);
            rect_size.x += amount;
            if(!updated_position)attached_node->position.x -= amount;
            updated_position = true;
            dirty_vertex_data = true;
            break;
        case RIGHT_BORDER:
            amount = std::max(amount,-rect_size.x);
            rect_size.x += amount;
            dirty_vertex_data = true;
            break;
        case TOP_BORDER:
            amount = std::max(amount,-rect_size.y);
            rect_size.y += amount;
            if(!updated_position)attached_node->position.y -= amount;
            updated_position = true;
            dirty_vertex_data = true;
            break;
        case BOTTOM_BORDER:
            amount = std::max(amount,-rect_size.y);
            rect_size.y += amount;
            dirty_vertex_data = true;
            break;
        default:
            saucer_err("Invalid border: " , border );
            break;
    }
    for( auto& child_rect : get_children_rects() ){
        if( child_rect->starts_on_viewport==false )
        for( int child_border : {LEFT_BORDER,RIGHT_BORDER,TOP_BORDER,BOTTOM_BORDER}){
            if( child_rect->is_border_anchored(child_border,border) ){
                if(child_border==border)    child_rect->grow(child_border,amount , updated_position);
                else                        child_rect->grow(child_border,-amount , updated_position);
            }
        }
    }
}
bool            AnchoredRect::is_hovered() const{
    return get_node() && get_node()->get_scene() && get_node()->get_scene()->get_current_hovered_anchored_rect() == this; // phew
}
bool            AnchoredRect::is_focused() const{
    return get_node() && get_node()->get_scene() && get_node()->get_scene()->get_current_focused_anchored_rect() == this; // phew
}
const std::vector<AnchoredRect*>    AnchoredRect::get_children_rects() const{
    std::vector<AnchoredRect*> ret;
    if( attached_node )
    for( auto& it : attached_node->get_children() ){
        AnchoredRect* rect = it->get_component<AnchoredRect>();
        if(rect)ret.push_back(rect);
    }
    return ret;
}
AnchoredRect*   AnchoredRect::get_parent_rect() const{
    if( attached_node && attached_node->get_parent() )
        return attached_node->get_parent()->get_component<AnchoredRect>();
    return nullptr;
}
bool            AnchoredRect::get_centered() const{
    return centered;
}
void            AnchoredRect::set_centered(bool new_val){
    if( centered!=new_val ){
        dirty_vertex_data = true;
        centered = new_val;
    }
}
Vector2         AnchoredRect::get_offset() const{
    return ( centered )?( get_rect_size()*-0.5 ):(offset);
}
void            AnchoredRect::set_offset(Vector2 new_val){
    if(!centered && offset!=new_val ){
        dirty_vertex_data = true;
        offset = new_val;
    }
}
void            AnchoredRect::cb_mouse_entered( ){
    //    
}
void            AnchoredRect::cb_mouse_exiting( ){
    //    
}
void            AnchoredRect::cb_mouse_button( Input::InputEventMouseButton& ev ){
    UNUSED(ev); 
}
void            AnchoredRect::cb_key( Input::InputEventKey& ev ){
    UNUSED(ev); 
}
void            AnchoredRect::cb_char( Input::InputEventChar& ev ){
    UNUSED(ev); 
}
void            AnchoredRect::bind_methods() {
    REGISTER_COMPONENT_HELPERS(AnchoredRect,"anchored_rect");

    REGISTER_LUA_CONSTANT( BORDER , TOP     ,   TOP_BORDER );
    REGISTER_LUA_CONSTANT( BORDER , LEFT    ,   LEFT_BORDER );
    REGISTER_LUA_CONSTANT( BORDER , RIGHT   ,   RIGHT_BORDER );
    REGISTER_LUA_CONSTANT( BORDER , BOTTOM  ,   BOTTOM_BORDER );

    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , is_border_anchored );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_anchored_border );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_rect_size );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_rect_size );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_offset );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_offset );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_centered );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_centered );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , is_hovered );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , is_focused );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_starts_on_viewport );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_starts_on_viewport );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_ignore_mouse );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_ignore_mouse );

}
void            AnchoredRect::push_editor_items(){
#ifdef SAUCER_EDITOR
    std::string border_names[] = {"LEFT_BORDER" , "RIGHT_BORDER" , "TOP_BORDER" , "BOTTOM_BORDER"};
    std::pair<int,int> border_pairs[] = {
        std::pair<int,int>(LEFT_BORDER,LEFT_BORDER),
        std::pair<int,int>(RIGHT_BORDER,RIGHT_BORDER),
        std::pair<int,int>(TOP_BORDER,TOP_BORDER),
        std::pair<int,int>(BOTTOM_BORDER,BOTTOM_BORDER),
        std::pair<int,int>(LEFT_BORDER,RIGHT_BORDER),
        std::pair<int,int>(RIGHT_BORDER,LEFT_BORDER),
        std::pair<int,int>(TOP_BORDER,BOTTOM_BORDER),
        std::pair<int,int>(BOTTOM_BORDER,TOP_BORDER)
    };
    for( auto& p : border_pairs ){
        bool b = is_border_anchored( p.first , p.second );
        char buf[64];
        sprintf(buf,"%s anchors to parent's %s?" , border_names[p.first].c_str() , border_names[p.second].c_str() );
        if( ImGui::Checkbox(buf,&b) ){
            set_anchored_border( p.first , p.second , b );
        }
    }

    PROPERTY_VEC2(this,rect_size);
    PROPERTY_BOOL(this,centered);
    PROPERTY_VEC2(this,offset);
    PROPERTY_BOOL(this,starts_on_viewport);
    ImGui::SameLine();
    PROPERTY_BOOL(this,ignore_mouse);
#endif
}
YamlNode        AnchoredRect::to_yaml_node() const {
    YamlNode ret;
    
    ret["rect_size"] = rect_size;
    ret["starts_on_viewport"] = starts_on_viewport;
    ret["ignore_mouse"] = ignore_mouse;
    ret["centered"] = centered;
    ret["offset"] = offset;
    for( int b : anchored_borders ) ret["anchored_borders"].push_back(b);

    return ret;
}
void            AnchoredRect::from_yaml_node( YamlNode yaml_node ) {

    SET_FROM_YAML_NODE_PROPERTY(yaml_node , rect_size );
    SET_FROM_YAML_NODE_PROPERTY(yaml_node , starts_on_viewport );
    SET_FROM_YAML_NODE_PROPERTY(yaml_node , ignore_mouse );
    SET_FROM_YAML_NODE_PROPERTY(yaml_node , offset );
    SET_FROM_YAML_NODE_PROPERTY(yaml_node , centered );

    for( size_t i = 0 ; i < 4 ; i++ ) anchored_borders[i] = static_cast<unsigned char>(yaml_node["anchored_borders"][i].as<int>());
}