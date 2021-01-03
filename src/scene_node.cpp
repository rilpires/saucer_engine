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
    lua_script = NULL;
}
SceneNode::~SceneNode(){
    get_out();
    for(auto it = children_nodes.begin() ; it != children_nodes.end() ; it++ )
        delete (*it);
    if( LuaEngine::current_actor == this ){
        LuaEngine::change_current_actor_env( nullptr );
    }
}

SceneNode*          SceneNode::lua_new(){ return new SceneNode(); }
void                SceneNode::set_position( const Vector2 new_pos ){ position = new_pos; }
Vector2             SceneNode::get_position( ) const{ return position; }
void                SceneNode::set_global_position( const Vector2 new_pos ){
    if( get_parent() ){
        Transform parent_transform = get_parent()->get_global_transform();
        set_position( parent_transform.inverted() * new_pos );
    } else set_position( new_pos );
}
Vector2             SceneNode::get_global_position() const{
    if( parent_node == NULL ) return position;
    else {
        Vector2 ret = parent_node->get_global_position();
        ret += position.rotated( parent_node->rotation_degrees );
        return ret;
    }
}
void                SceneNode::set_rotation_degrees( float new_rotation_degrees_cw ){
    rotation_degrees=new_rotation_degrees_cw;
}
float               SceneNode::get_rotation_degrees( ) const {
    return rotation_degrees;
}
float               SceneNode::get_global_rotation_degrees() const{
    if(!parent_node) return rotation_degrees;
    else return parent_node->get_global_rotation_degrees() + rotation_degrees;
}
Transform           SceneNode::get_transform() const{
    Transform ret;
    return ret.scale(Vector2(1,1))
              .rotate_z( get_rotation_degrees() )  
              .translate( Vector3( position.x , position.y , ((float)z)/SHORT_MAX ) );
}
Transform           SceneNode::get_global_transform() const{
    Transform ret;
    Vector2 global_pos = get_global_position();
    return ret.scale(Vector2(1,1))
              .rotate_z( get_global_rotation_degrees() )  
              .translate( Vector3( global_pos.x , global_pos.y , ((float)get_global_z())/SHORT_MAX ) );
}
void                SceneNode::set_z( short new_z ){z=new_z;}
short               SceneNode::get_z( ) const {return z;}
short               SceneNode::get_global_z() const {
    if( relative_z && parent_node )
        return parent_node->get_global_z()+z;
    return z;
}
void                SceneNode::set_relative_z(bool new_val){relative_z=new_val;}
bool                SceneNode::is_z_relative() const {return relative_z;}
LuaScriptResource*  SceneNode::get_script() const { return lua_script;};
void                SceneNode::set_script( LuaScriptResource* ls ){
    if( lua_script ){
        std::cerr << "Hmm you shouldn't change scripts attached in a node once set..." << std::endl;
    } else {
        lua_script = ls;
        LuaEngine::create_actor_env( this );
    }
}
void                SceneNode::get_out(){
    if( get_scene()->get_root_node() == this ){
        get_scene()->set_root_node(nullptr);
        exited_tree();
    }
    if( parent_node ){
        std::vector<SceneNode*>& parent_chidren = parent_node->children_nodes;
        for( auto it = parent_chidren.begin() ; it != parent_chidren.end() ; it++ ){
            if( *it == this ){
                parent_chidren.erase( it );
                parent_node = nullptr;
                scene = nullptr;
                exited_tree();
                break; 
            }
        }
    }
}
void                SceneNode::add_child( SceneNode* p_child_node ){
    if(p_child_node->parent_node) std::cerr << "Trying to add a node as a child but it already has a parent. " << std::endl; 
    else{
        p_child_node->parent_node = this;
        children_nodes.push_back( p_child_node );
        p_child_node->entered_tree();
    }

}
SceneNode*          SceneNode::get_parent( ) const { return parent_node;};
Scene*              SceneNode::get_scene() const{
    if(parent_node) return parent_node->get_scene();
    else return scene;
}
std::vector<SceneNode*> const&  SceneNode::get_children() const { return children_nodes; }

template<typename T> T*     SceneNode::get_component() const {
    return T::recover_from_node(this);
}
template<typename T> 
T*                          SceneNode::create_component( ){
    if( get_component<T>() ){
        std::cout << "Warning: trying to create a component of a type that already exists" << std::endl;
        return nullptr;
    }
    else {
        T* new_comp = new T();
        attached_components.push_back(new_comp);
        ((Component*)new_comp)->attach_node(this);
        return new_comp;
    }
}
template<> 
CollisionBody*              SceneNode::create_component( ){
    if( get_component<CollisionBody>() ){
        std::cout << "Warning: trying to create a component of a type that already exists" << std::endl;
        return nullptr;
    }
    else {
        CollisionBody* new_comp = new CollisionBody();
        attached_components.push_back(new_comp);
        ((Component*)new_comp)->attach_node(this);
        new_comp->tree_changed();
        return new_comp;
    }
}
template<typename T> 
void                          SceneNode::destroy_component( ){
    T* current_comp = get_component<T>();
    if( !current_comp ){
        std::cout << "Warning: trying to destroy an unexistent component " << std::endl;
    } else {
        auto it = attached_components.begin();
        while( *it != (Component*)current_comp ) it++;
        attached_components.erase(it);
        delete current_comp;
    }
}
void        SceneNode::entered_tree(){
    LuaEngine::execute_entered_tree( this );
    CollisionBody* body = get_component<CollisionBody>();
    if( body ) body->tree_changed();
}
void        SceneNode::exited_tree(){
    LuaEngine::execute_exited_tree( this );
    CollisionBody* body = get_component<CollisionBody>();
    if( body ) body->tree_changed();
}
void        SceneNode::bind_methods(){
    
    REGISTER_LUA_NESTED_STATIC_FUNCTION(SceneNode,lua_new);
    
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_position);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_position);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_global_position);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_global_position);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_rotation_degrees);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_rotation_degrees);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_global_rotation_degrees);
    // REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_global_transform);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_z);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_z);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_global_z);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_relative_z);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,is_z_relative);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_script);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_script);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_out);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,add_child);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_parent);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_scene);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_children);
                                                                       

    REGISTER_COMPONENT_HELPERS(Sprite,"sprite");                  
    REGISTER_COMPONENT_HELPERS(CollisionBody,"body");

}


