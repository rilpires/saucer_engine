#include "scene_node.h"
#include "core.h"
#include <iostream>
#include <type_traits>

#define SHORT_MAX 0b0111111111111111


SceneNode::SceneNode(){
    position = Vector2(0,0);
    rotation_degrees = 0.0f;
    z = 0;
    relative_z = true;
    parent_node = NULL;
    scene = NULL;
    image_texture = NULL;
    lua_script = NULL;
}
SceneNode::~SceneNode(){
    for(auto it = children_nodes.begin() ; it != children_nodes.end() ; it++ )
        delete (*it);
}
void        SceneNode::set_position( const Vector2 new_pos ){
    position = new_pos;
}
Vector2     SceneNode::get_position( ) const{
    return position;
}
Vector2     SceneNode::get_global_position() const{
    if( parent_node == NULL ) return position;
    else {
        Vector2 ret = parent_node->get_global_position();
        ret += position.rotated( parent_node->rotation_degrees );
        return ret;
    }
}
float       SceneNode::get_global_rotation_degrees() const{
    if(!parent_node) return rotation_degrees;
    else{
        return parent_node->get_global_rotation_degrees() + rotation_degrees;
    }
}
Transform   SceneNode::get_global_transform() const{
    Transform ret;
    Vector2 global_pos = get_global_position();
    return ret.scale(Vector2(1,1))
              .rotate_z( get_global_rotation_degrees() )  
              .translate( Vector3( global_pos.x , global_pos.y , ((float)get_global_z())/SHORT_MAX ) );
}
short       SceneNode::get_global_z() const {
    if( relative_z && parent_node )
        return parent_node->get_global_z()+z;
    return z;
}
void        SceneNode::set_script_resource( LuaScriptResource* ls ){
    if( lua_script ){
        std::cerr << "Hmm you shouldn't change scripts attached in a node once set..." << std::endl;
    } else {
        lua_script = ls;
        LuaEngine::create_actor_env( this );
    }
}
void        SceneNode::get_out(){
    if( parent_node ){
        std::vector<SceneNode*>& parent_chidren = parent_node->children_nodes;
        for( auto it = parent_chidren.begin() ; it != parent_chidren.end() ; it++ ){
            if( *it == this ){
                parent_chidren.erase( it );
                break; 
            }
        }
    }
    parent_node = nullptr;
    scene = nullptr;
}
void        SceneNode::add_child_node( SceneNode* p_child_node ){
    if(p_child_node->parent_node) std::cerr << "Trying to add a node as a child but it already has a parent. " << std::endl; 
    else{
        p_child_node->parent_node = this;
        children_nodes.push_back( p_child_node );
    }

}
std::vector<SceneNode*> const&  SceneNode::get_children() const {
    return std::forward<std::vector<SceneNode*> const&&>(children_nodes);
}
Scene*      SceneNode::get_scene() const{
    if(parent_node) return parent_node->get_scene();
    else return scene;
}

void        SceneNode::bind_methods(){
    
    // LuaEngine::register_function("SceneNode","get_position", LuaEngine::to_lua_cfunction<pointer_without_const<decltype(&SceneNode::get_position)>::type>::generate_lambda<(&SceneNode::get_position)>() );

    REGISTER_LUA_FUNCTION(SceneNode,get_position);
    REGISTER_LUA_FUNCTION(SceneNode,set_position);
    REGISTER_LUA_FUNCTION(SceneNode,get_global_position);
    REGISTER_LUA_FUNCTION(SceneNode,get_rotation_degrees);
    REGISTER_LUA_FUNCTION(SceneNode,set_rotation_degrees);
    REGISTER_LUA_FUNCTION(SceneNode,get_global_rotation_degrees);
    REGISTER_LUA_FUNCTION(SceneNode,get_z);
    REGISTER_LUA_FUNCTION(SceneNode,get_global_z);
    REGISTER_LUA_FUNCTION(SceneNode,is_z_relative);
    REGISTER_LUA_FUNCTION(SceneNode,get_image_texture);
    REGISTER_LUA_FUNCTION(SceneNode,get_script_resource);
    REGISTER_LUA_FUNCTION(SceneNode,get_parent);
    REGISTER_LUA_FUNCTION(SceneNode,get_children);
    REGISTER_LUA_FUNCTION(SceneNode,get_scene);

    
}

CameraNode::~CameraNode(){
    set_current(false);
}
bool        CameraNode::is_current(){
    return get_scene()->get_current_camera() == this;
}
void        CameraNode::set_current( bool current ){
    if( current ){
        get_scene()->set_current_camera(this);
    }
    else if ( is_current() ) {
        get_scene()->set_current_camera(NULL);
    }
}
void        CameraNode::entered_scene(){
}

