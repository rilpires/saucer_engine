#include "camera.h"
#include "lua_engine.h"
#include "scene_node.h"
#include "scene.h"

std::unordered_multimap<SaucerId,Camera*> Camera::component_from_node;

Camera::Camera() {
    zoom = Vector2(1,1);
    left_limit   = -999999;
    right_limit  =  999999;
    top_limit    =  999999;
    bottom_limit = -999999;   
}
Camera::~Camera(){
    if( get_node() && get_node()->get_scene() ) set_active(false);
}
Vector2 Camera::get_zoom() const {
    return zoom;
}
void Camera::set_zoom(Vector2 new_val) {
    zoom = new_val;
}
float Camera::get_left_limit() const {
    return left_limit;
}
void Camera::set_left_limit(float new_val) {
    left_limit = new_val;
}
float Camera::get_right_limit() const {
    return right_limit;
}
void Camera::set_right_limit(float new_val) {
    right_limit = new_val;
}
float Camera::get_top_limit() const {
    return top_limit;
}
void Camera::set_top_limit(float new_val) {
    top_limit = new_val;
}
float Camera::get_bottom_limit() const {
    return bottom_limit;
}
void Camera::set_bottom_limit(float new_val) {
    bottom_limit = new_val;
}
bool    Camera::get_active() const {
    SceneNode* node = get_node();
    return node && node->get_scene() && node->get_scene()->get_current_camera()==this;
}
void    Camera::set_active( bool new_val){
    SceneNode* node = get_node();
    if( node && node->get_scene() ){
        node->get_scene()->set_current_camera( (new_val)?(this):(nullptr) );
    } else saucer_warn("Can't set a camera as active without it being inside a scene.");
}
void    Camera::entered_tree(){
    if( get_node() && 
    get_node()->get_scene() && 
    get_node()->get_scene()->get_current_camera()==0 ){
        get_node()->get_scene()->set_current_camera( this );
    }
}
void    Camera::exiting_tree(){
    set_active(false);
}
void    Camera::bind_methods() {
    
    REGISTER_LUA_MEMBER_FUNCTION( Camera , get_zoom );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , set_zoom );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , get_left_limit );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , set_left_limit );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , get_right_limit );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , set_right_limit );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , get_top_limit );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , set_top_limit );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , get_bottom_limit );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , set_bottom_limit );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , get_active );
    REGISTER_LUA_MEMBER_FUNCTION( Camera , set_active );

}
