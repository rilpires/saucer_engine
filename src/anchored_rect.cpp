#include "anchored_rect.h"
#include "lua_engine.h"
#include "scene.h"

std::unordered_multimap<SaucerId,AnchoredRect*> AnchoredRect::component_from_node;

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
    if( attached_node && attached_node->get_scene() && attached_node->get_scene()->get_current_hovered_anchored_rect() == this ){
        attached_node->get_scene()->set_current_hovered_anchored_rect(nullptr);
    }
}
std::vector<RenderData>  AnchoredRect::generate_render_data(){
    std::vector<RenderData> ret;
    return ret;
}
bool AnchoredRect::is_border_anchored( int border , int parent_border ) const {
    return (anchored_borders[border] & (1<<(parent_border-1)));
}
void AnchoredRect::set_anchored_border( int border , int parent_border , bool new_val ){
    if (new_val)    anchored_borders[border] |=  (1<<(parent_border-1));
    else            anchored_borders[border] &= ~(1<<(parent_border-1));
}
Vector2 AnchoredRect::get_rect_pos() const {
    return rect_pos;
}
void AnchoredRect::set_rect_pos(Vector2 new_val) {
    rect_pos = new_val;
}
Vector2 AnchoredRect::get_rect_size() const {
    return rect_size;
}
void AnchoredRect::set_rect_size(Vector2 new_val) {
    new_val.x = std::max(0.0f,new_val.x);
    new_val.y = std::max(0.0f,new_val.y);        
    grow(RIGHT_BORDER,new_val.x - rect_size.x);
    grow(BOTTOM_BORDER,new_val.y - rect_size.y);
}
bool    AnchoredRect::get_starts_on_viewport() const {
    return starts_on_viewport || ( use_scene_node_transform==false && get_parent_rect()==nullptr );
}
void    AnchoredRect::set_starts_on_viewport(bool new_val) {
    starts_on_viewport = new_val;
}
bool    AnchoredRect::get_ignore_mouse() const{
    return ignore_mouse;
}
void    AnchoredRect::set_ignore_mouse(bool new_val){
    ignore_mouse = new_val;
}
bool    AnchoredRect::get_use_scene_node_transform() const {
    return use_scene_node_transform;
}
void AnchoredRect::set_use_scene_node_transform(bool new_val) {
    use_scene_node_transform = new_val;
}
Vector2 AnchoredRect::get_global_rect_pos() const{
    return get_parent_global_transform() * rect_pos;
}
void AnchoredRect::grow(int border, float amount) {
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
AnchoredRect*                       AnchoredRect::get_parent_rect() const{
    if( attached_node && attached_node->get_parent() )
        return attached_node->get_parent()->get_component<AnchoredRect>();
    return nullptr;
}
Transform   AnchoredRect::get_parent_global_transform() const{
    AnchoredRect* parent_rect = get_parent_rect();
    if( starts_on_viewport ){
        return Transform() ;
    } else if( use_scene_node_transform ){
        return get_node()->get_global_transform();
    } else if( parent_rect ){
        return parent_rect->get_parent_global_transform().translate(parent_rect->rect_pos);
    } else {
        // Well.. the same as starts_on_viewport
        return Transform();
    }
}
void AnchoredRect::bind_methods() {
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
