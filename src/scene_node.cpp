#include "scene_node.h"
#include "core.h"

#define SHORT_MAX 0b0111111111111111

SceneNode::SceneNode(){
    position = Vector2(0,0);
    scale = Vector2(1,1);
    rotation_degrees = 0.0f;
    z = 0;
    name = std::to_string(get_saucer_id());
    relative_z = true;
    modulate = Color();
    self_modulate = Color();
    inherits_transform = true;
    parent_node = NULL;
    scene = NULL;
    lua_script = NULL;
    visible = true;
}
SceneNode::~SceneNode(){
    for( Component* component : attached_components ){
        component->erase_from_component_map();
        delete component;
    }
    attached_components.clear();
    for( size_t i = 0 ; i < children_nodes.size() ; i++ ) 
        delete children_nodes[i];
    children_nodes.clear();
    if( LuaEngine::current_actor == this ){
        LuaEngine::change_current_actor_env( nullptr );
    }
    get_out();
}
void                SceneNode::set_scene(Scene* new_scene){
    scene = new_scene;
    for( auto& child : children_nodes ) child->set_scene(new_scene);
}
SceneNode*          SceneNode::lua_new(){ return new SceneNode(); }
std::string         SceneNode::get_name() const{
    return name;
}
void                SceneNode::set_name( std::string new_val ){
    name = new_val;
}
void                SceneNode::set_position( const Vector2 new_pos ){ position = new_pos; }
Vector2             SceneNode::get_position( ) const{ return position; }
void                SceneNode::set_scale( const Vector2 new_scale ){
    scale = new_scale;
}
Vector2             SceneNode::get_scale( ) const{
    return scale;
}
void                SceneNode::set_global_position( const Vector2 new_pos ){
    if( get_parent() ){
        Transform parent_transform = get_parent()->get_global_transform();
        set_position( parent_transform.inverted() * new_pos );
    } else set_position( new_pos );
}
Vector2             SceneNode::get_global_position() const{
    if( parent_node == NULL ) return position;
    else return parent_node->get_global_transform() * position;
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
    return Transform().scale(scale)
                      .rotate_z( rotation_degrees )  
                      .translate( Vector3( position.x , position.y , ((float)z)/SHORT_MAX ) );
}
Transform           SceneNode::get_global_transform() const{
    Transform ret = get_transform();
    if( inherits_transform && parent_node ) return parent_node->get_global_transform() * ret;
    else return ret;
}
void                SceneNode::set_z( short new_z ){z=new_z;}
short               SceneNode::get_z( ) const {return z;}
short               SceneNode::get_global_z() const {
    if( relative_z && parent_node )
        return parent_node->get_global_z()+z;
    return z;
}
Color               SceneNode::get_modulate() const {
    return modulate;
}
void                SceneNode::set_modulate( Color new_col ){
    modulate = new_col;
}
Color               SceneNode::get_self_modulate() const {
    return self_modulate;
}
void                SceneNode::set_self_modulate( Color new_col ){
    self_modulate = new_col;
}
Color               SceneNode::get_global_modulate() const{
    Color ret = self_modulate;
    const SceneNode* node = this;
    while(node){
        ret *= node->modulate;
        node = node->get_parent();
    }
    return ret;
}
void                SceneNode::set_relative_z(bool new_val){relative_z=new_val;}
bool                SceneNode::is_z_relative() const {return relative_z;}
bool                SceneNode::is_visible() const{
    return visible;
}
void                SceneNode::set_visible( bool new_val ){
    visible = new_val;
}
LuaScriptResource*  SceneNode::get_script() const { return lua_script;};
void                SceneNode::set_script( LuaScriptResource* ls ){
    if( lua_script ){
        saucer_err( "Hmm you shouldn't change scripts attached in a node once set..." )
    } else {
        lua_script = ls;
        LuaEngine::create_actor_env( this );
    }
}
void                SceneNode::get_out(){
    bool actually_got_out = get_scene() || parent_node;
    if( get_scene() && get_scene()->get_root_node() == this ){
        get_scene()->set_root_node(nullptr);
    }
    if( parent_node ){
        std::vector<SceneNode*>& parent_chidren = parent_node->children_nodes;
        for( auto it = parent_chidren.begin() ; it != parent_chidren.end() ; it++ ){
            if( *it == this ){
                parent_chidren.erase( it );
                parent_node = nullptr;
                break; 
            }
        }
    }
    if( actually_got_out ){
        exiting_tree();
    }
    set_scene(nullptr);
}
void                SceneNode::add_child( SceneNode* p_child_node ){
    if(p_child_node->parent_node) saucer_err( "Trying to add a node as a child but it already has a parent. " ) 
    else{
        p_child_node->parent_node = this;
        children_nodes.push_back( p_child_node );
        p_child_node->set_scene(scene);
        if( scene ){
            p_child_node->entered_tree();
        }
    }

}
SceneNode*          SceneNode::get_parent( ) const { return parent_node;};
Scene*              SceneNode::get_scene() const{
    return scene;
}
void                SceneNode::queue_free(){
    Engine::get_current_scene()->queue_free_node(this);
}
std::vector<SceneNode*> const&  SceneNode::get_children() const { 
    return children_nodes; 
}
bool                SceneNode::get_inherits_transform() const{
    return inherits_transform;
}
void                SceneNode::set_inherits_transform(bool new_val){
    inherits_transform = new_val;
}
void        SceneNode::entered_tree(){
    for( auto& child : children_nodes ) child->entered_tree();
    for( Component*& c : attached_components )
        c->entered_tree();
    LuaEngine::execute_callback( "entered_tree", this );
    
}
void        SceneNode::exiting_tree(){
    for( auto& child : children_nodes ) child->exiting_tree();
    LuaEngine::execute_callback( "exiting_tree", this );
    for( Component*& c : attached_components )
        c->exiting_tree();
}
void        SceneNode::bind_methods(){
    
    REGISTER_LUA_NESTED_STATIC_FUNCTION(SceneNode,lua_new);
    
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_name);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_name);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_position);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_position);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_scale);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_scale);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_global_position);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_global_position);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_rotation_degrees);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_rotation_degrees);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_global_rotation_degrees);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_z);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_z);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_global_z);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_relative_z);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,is_z_relative);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_modulate );
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_modulate );
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_self_modulate );
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_self_modulate );
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_visible);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,is_visible);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_script);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_script);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_out);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,add_child);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_parent);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_scene);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_children);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,queue_free);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_children);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,queue_free);
    
    REGISTER_COMPONENT_HELPERS(Sprite,"sprite");
    REGISTER_COMPONENT_HELPERS(Camera,"camera");
    REGISTER_COMPONENT_HELPERS(CollisionBody,"body");
    REGISTER_COMPONENT_HELPERS(AudioEmitter,"audio_emitter");
    REGISTER_COMPONENT_HELPERS(AnchoredRect,"anchored_rect");
    REGISTER_COMPONENT_HELPERS(PatchRect,"patch_rect");
    REGISTER_COMPONENT_HELPERS(LabelRect,"label_rect");

}


