#include "anchored_rect.h"
#include "lua_engine.h"

std::unordered_multimap<SaucerId,AnchoredRect*> AnchoredRect::component_from_node;

AnchoredRect::AnchoredRect() {
    anchored_borders = 0;
    rect_pos = Vector2(0,0);
    rect_size = Vector2(0,0);
    starts_on_viewport = false;
    use_scene_node_transform = false;
}


std::vector<RenderData>  AnchoredRect::generate_render_data() const{
    std::vector<RenderData> ret;
    return ret;
}
bool AnchoredRect::is_border_anchored( int border ) const {
    return (anchored_borders & (1<<(border-1)));    
}
void AnchoredRect::set_anchored_border(int border, bool new_val) {
    if (new_val)    anchored_borders |=  (1<<(border-1));
    else            anchored_borders &= ~(1<<(border-1));
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

bool AnchoredRect::get_starts_on_viewport() const {
    return starts_on_viewport;
}

void AnchoredRect::set_starts_on_viewport(bool new_val) {
    starts_on_viewport = new_val;
}

bool AnchoredRect::get_use_scene_node_transform() const {
    return use_scene_node_transform;
}

void AnchoredRect::set_use_scene_node_transform(bool new_val) {
    use_scene_node_transform = new_val;
}
Vector2 AnchoredRect::get_global_rect_pos() const{
    AnchoredRect* parent_rect = get_parent_rect();
    Vector2 local_pos = rect_pos*Vector2(1,-1);
    if( starts_on_viewport ){
        return local_pos + Engine::get_window_size() * Vector2(-0.5,0.5) ;
    } else if( parent_rect ){
        return parent_rect->get_global_rect_pos() + local_pos;
    } else if( use_scene_node_transform ){
        return get_node()->get_global_transform() * local_pos;
    } else {
        // Well.. the same as starts_on_viewport
        return local_pos + Engine::get_window_size() * Vector2(-0.5,0.5) ;
    }
}
void AnchoredRect::grow(int border, float amount) {
    switch (border)
    {
        case LEFT_BORDER:
            rect_size.x += amount;
            rect_pos.x -= amount;
            break;
        case RIGHT_BORDER:
            rect_size.x += amount;
            break;
        case TOP_BORDER:
            rect_size.y += amount;
            rect_pos.y -= amount;
            break;
        case BOTTOM_BORDER:
            rect_size.y += amount;
            break;
        default:
            saucer_err("Invalid border: " , border );
            break;
    }
    for( auto& it : get_children_rects() ){
        if( it->starts_on_viewport==false && it->is_border_anchored(border))
            it->grow(border,amount);
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
void AnchoredRect::bind_methods() {
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , is_border_anchored );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_anchored_border );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_rect_pos );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_rect_pos );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_rect_size );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_rect_size );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_starts_on_viewport );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_starts_on_viewport );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_use_scene_node_transform );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , set_use_scene_node_transform );
    REGISTER_LUA_MEMBER_FUNCTION( AnchoredRect , get_global_rect_pos );    
}
