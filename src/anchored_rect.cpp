#include "anchored_rect.h"
#include "core.h"

AnchoredRect::AnchoredRect() {
    anchored_borders[0] = 0;
    anchored_borders[1] = 0;
    anchored_borders[2] = 0;
    anchored_borders[3] = 0;
    rect_pos = Vector2(0,0);
    rect_size = Vector2(0,0);
    starts_on_viewport = false;
    ignore_mouse = true;
    use_scene_node_transform = false;
}
AnchoredRect::~AnchoredRect() {
    if( attached_node && attached_node->get_scene() ){
        Scene* scene = attached_node->get_scene();
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
Vector2         AnchoredRect::get_rect_pos() const {
    return rect_pos;
}
void            AnchoredRect::set_rect_pos(Vector2 new_val) {
    rect_pos = new_val;
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
    return starts_on_viewport || ( use_scene_node_transform==false && get_parent_rect()==nullptr );
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
bool            AnchoredRect::get_use_scene_node_transform() const {
    return use_scene_node_transform;
}
void            AnchoredRect::set_use_scene_node_transform(bool new_val) {
    use_scene_node_transform = new_val;
}
Vector2         AnchoredRect::get_global_rect_pos() const{
    return get_parent_global_transform() * rect_pos;
}
void            AnchoredRect::grow(int border, float amount) {
    if( amount == 0 ) return;
    switch (border)
    {
        case LEFT_BORDER:
            amount = std::max(amount,-rect_size.x);
            rect_size.x += amount;
            rect_pos.x -= amount;
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
            rect_pos.y -= amount;
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
                if(child_border==border)    child_rect->grow(child_border,amount);
                else                        child_rect->grow(child_border,-amount);
            }
        }
    }
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
Transform       AnchoredRect::get_parent_global_transform() const{
    AnchoredRect* parent_rect = get_parent_rect();
    if( starts_on_viewport ){
        return Engine::get_render_engine()->get_camera_transform().translate( Engine::get_render_engine()->get_viewport_rect().get_size()*0.5 ); // this is fucked up
    } else if( use_scene_node_transform ){
        return get_node()->get_global_transform();
    } else if( parent_rect ){
        return parent_rect->get_parent_global_transform().translate(parent_rect->rect_pos);
    } else {
        // Well.. the same as starts_on_viewport
        return Engine::get_render_engine()->get_camera_transform();
    }
}
void            AnchoredRect::bind_methods() {
    REGISTER_LUA_CONSTANT( BORDER , TOP     ,   TOP_BORDER );
    REGISTER_LUA_CONSTANT( BORDER , LEFT    ,   LEFT_BORDER );
    REGISTER_LUA_CONSTANT( BORDER , RIGHT   ,   RIGHT_BORDER );
    REGISTER_LUA_CONSTANT( BORDER , BOTTOM  ,   BOTTOM_BORDER );

    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , is_border_anchored );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_anchored_border );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_rect_pos );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_rect_pos );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_rect_size );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_rect_size );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_starts_on_viewport );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_starts_on_viewport );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_ignore_mouse );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_ignore_mouse );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_use_scene_node_transform );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_use_scene_node_transform );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_global_rect_pos );    
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

    PROPERTY_VEC2(this,rect_pos);
    PROPERTY_VEC2(this,rect_size);
    PROPERTY_BOOL(this,starts_on_viewport);
    ImGui::SameLine();
    PROPERTY_BOOL(this,use_scene_node_transform);
    ImGui::SameLine();
    PROPERTY_BOOL(this,ignore_mouse);
#endif
}
YamlNode        AnchoredRect::to_yaml_node() const {
    YamlNode ret;
    
    ret["rect_pos"] = rect_pos;
    ret["rect_size"] = rect_size;
    ret["starts_on_viewport"] = starts_on_viewport;
    ret["ignore_mouse"] = ignore_mouse;
    ret["use_scene_node_transform"] = use_scene_node_transform;
    for( int b : anchored_borders ) ret["anchored_borders"].push_back(b);

    return ret;
}
void            AnchoredRect::from_yaml_node( YamlNode yaml_node ) {

    set_rect_pos( yaml_node["rect_pos"].as<decltype(rect_pos)>() );
    set_rect_size( yaml_node["rect_size"].as<decltype(rect_size)>() );
    set_starts_on_viewport( yaml_node["starts_on_viewport"].as<decltype(starts_on_viewport)>() );
    set_ignore_mouse( yaml_node["ignore_mouse"].as<decltype(ignore_mouse)>() );
    set_use_scene_node_transform( yaml_node["use_scene_node_transform"].as<decltype(use_scene_node_transform)>() );
    
    for( size_t i = 0 ; i < 4 ; i++ ) anchored_borders[i] = static_cast<unsigned char>(yaml_node["anchored_borders"][i].as<int>());
}