#include "scene_node.h"
#include "core.h"

#define SHORT_MAX 0b0111111111111111

std::unordered_map<std::string, SceneNode::ComponentConstructor > SceneNode::__component_constructors;
        
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
    std::vector<SceneNode*> children_nodes_copy = children_nodes;
    for( size_t i = 0 ; i < children_nodes_copy.size() ; i++ ) 
        delete children_nodes_copy[i]; // they will call get_out
    children_nodes.clear(); // probably unnecessary tho
    for( Component* component : attached_components ){
        component->erase_from_component_map();
        delete component;
    }
    attached_components.clear();
    get_out();
    LuaEngine::destroy_actor_env(this);
}
void                    SceneNode::set_scene(Scene* new_scene){
    scene = new_scene;
    for( auto& child : children_nodes ) child->set_scene(new_scene);
}
SceneNode*              SceneNode::lua_new(){ return new SceneNode(); }
std::string             SceneNode::get_name() const{
    return name;
}
void                    SceneNode::set_name( std::string new_val ){
    name = new_val;
}
void                    SceneNode::set_position( const Vector2 new_pos ){ position = new_pos; }
Vector2                 SceneNode::get_position( ) const{ return position; }
void                    SceneNode::set_scale( const Vector2 new_scale ){
    scale = new_scale;
}
Vector2                 SceneNode::get_scale( ) const{
    return scale;
}
void                    SceneNode::set_global_position( const Vector2 new_pos ){
    if( get_parent() ){
        Transform parent_transform = get_parent()->get_global_transform();
        set_position( parent_transform.inverted() * new_pos );
    } else set_position( new_pos );
}
Vector2                 SceneNode::get_global_position() const{
    if( parent_node == NULL ) return position;
    else return parent_node->get_global_transform() * position;
}
void                    SceneNode::set_rotation_degrees( float new_rotation_degrees_cw ){
    rotation_degrees=new_rotation_degrees_cw;
}
float                   SceneNode::get_rotation_degrees( ) const {
    return rotation_degrees;
}
float                   SceneNode::get_global_rotation_degrees() const{
    if(!parent_node) return rotation_degrees;
    else return parent_node->get_global_rotation_degrees() + rotation_degrees;
}
Transform               SceneNode::get_transform() const{
    return Transform().scale(scale)
                      .rotate_z( rotation_degrees )  
                      .translate( Vector3( position.x , position.y , ((float)z)/SHORT_MAX ) );
}
Transform               SceneNode::get_global_transform() const{
    Transform ret = get_transform();
    if( inherits_transform && parent_node ){
        Transform parent_global_transform = parent_node->get_global_transform();
        if( !relative_z ) parent_global_transform.m[11] = 0;
        ret = parent_global_transform * ret;
    }
    return ret;
}
void                    SceneNode::set_z( short new_z ){z=new_z;}
short                   SceneNode::get_z( ) const {return z;}
short                   SceneNode::get_global_z() const {
    if( relative_z && parent_node )
        return parent_node->get_global_z()+z;
    return z;
}
Color                   SceneNode::get_modulate() const {
    return modulate;
}
void                    SceneNode::set_modulate( Color new_col ){
    modulate = new_col;
}
Color                   SceneNode::get_self_modulate() const {
    return self_modulate;
}
void                    SceneNode::set_self_modulate( Color new_col ){
    self_modulate = new_col;
}
Color                   SceneNode::get_global_modulate() const{
    Color ret = self_modulate;
    const SceneNode* node = this;
    while(node){
        ret *= node->modulate;
        node = node->get_parent();
    }
    return ret;
}
void                    SceneNode::set_relative_z(bool new_val){relative_z=new_val;}
bool                    SceneNode::get_relative_z() const {return relative_z;}
bool                    SceneNode::get_visible() const{
    return visible;
}
void                    SceneNode::set_visible( bool new_val ){
    visible = new_val;
}
LuaScriptResource*      SceneNode::get_script() const { return lua_script;};
void                    SceneNode::set_script( LuaScriptResource* ls ){
    if( !Engine::is_editor() && ls && lua_script && ls != lua_script ){
        saucer_err( "Hmm you shouldn't runtime change scripts attached in a node once set..." );
    }
    lua_script = ls;
    if(!Engine::is_editor() ){
        if( lua_script )LuaEngine::create_actor_env( this );
    } else {
        // validate script maybe
    }
}
bool                    SceneNode::is_parent_of( SceneNode* other ) const{
    while( other->get_parent() ){
        if( other->get_parent() == this ) return true;
        other = other->get_parent();
    }
    return false;
}
void                    SceneNode::get_out(){
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
    if( get_scene() ){
        exiting_tree();
        set_scene(nullptr);
    }
}
void                    SceneNode::add_child( SceneNode* p_child_node ){
    SAUCER_ASSERT(p_child_node!=nullptr , "Trying to add a child SceneNode but it is null.");
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
SceneNode*              SceneNode::get_node( std::string child_name ) const{
    for( auto c : children_nodes ) if(c->get_name()==child_name) return c;
    return nullptr;
}
SceneNode*              SceneNode::find_node( std::string child_name ) const{
    for( auto c : children_nodes ) if(c->get_name()==child_name) return c;
    for( auto c : children_nodes ){
        SceneNode* node = c->find_node(child_name);
        if( node ) return node;
    }
    return nullptr;
}
SceneNode*              SceneNode::get_parent( ) const { return parent_node;};
Scene*                  SceneNode::get_scene() const{
    return scene;
}
void                    SceneNode::queue_free(){
    Engine::get_current_scene()->queue_free_node(this);
}
NodeTemplateResource*   SceneNode::pack_as_resource() const{
    NodeTemplateResource* ret = new NodeTemplateResource();
    ret->yaml_node = to_yaml_node();
    return ret;
}
SceneNode*              SceneNode::duplicate() const{
    SceneNode* ret = new SceneNode();
    ret->from_yaml_node( to_yaml_node() );
    return ret;
}
std::vector<SceneNode*> const&  SceneNode::get_children() const { 
    return children_nodes; 
}
bool                    SceneNode::get_inherits_transform() const{
    return inherits_transform;
}
void                    SceneNode::set_inherits_transform(bool new_val){
    inherits_transform = new_val;
}
void                    SceneNode::destroy_component( Component* c ){
    SAUCER_ASSERT(c->get_node()==this , "Trying to destroy a component that isn't attached to the right SceneNode.");
    auto it = attached_components.begin();
    for( ; it != attached_components.end() ; it++ )
        if( *it == c ) break;
    if( it == attached_components.end() ){
        saucer_err("Trying to destroy a component that isn't attached to a SceneNode.");
        return;
    }
    attached_components.erase(it);
    (*it)->erase_from_component_map();
    delete (*it);
}
std::vector<Component*> SceneNode::get_attached_components() const{
    std::vector<Component*> ret;
    for( Component* c : attached_components ) ret.push_back(c);
    return ret;
}
const std::unordered_map<std::string, SceneNode::ComponentConstructor >& SceneNode::__get_component_constructors() {
    return __component_constructors;
}
void        SceneNode::__register_component_constructor( std::string name , ComponentConstructor c ){
    __component_constructors[name] = c;
}
void        SceneNode::entered_tree(){
    for( auto& child : children_nodes ) child->entered_tree();
    for( Component*& c : attached_components )
        c->entered_tree();
    if(!Engine::is_editor() ) LuaEngine::execute_callback( "entered_tree", this );
    
}
void        SceneNode::exiting_tree(){
    for( auto& child : children_nodes ) child->exiting_tree();
    if(!Engine::is_editor() ) LuaEngine::execute_callback( "exiting_tree", this );
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
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_relative_z);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_modulate );
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_modulate );
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_self_modulate );
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_self_modulate );
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_visible);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_visible);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,set_script);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_script);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_out);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,is_parent_of);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,add_child);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_node);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,find_node);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_parent);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_scene);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_children);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,queue_free);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,get_children);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,queue_free);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,pack_as_resource);
    REGISTER_LUA_MEMBER_FUNCTION(SceneNode,duplicate);
    
}
YamlNode    SceneNode::to_yaml_node() const{
    YamlNode ret;
    
    ret["name"] = name;
    ret["position"] = position;
    ret["scale"] = scale;
    ret["rotation_degrees"] = rotation_degrees;
    ret["z"] = z;
    ret["relative_z"] = relative_z;
    ret["modulate"] = modulate;
    ret["self_modulate"] = self_modulate;
    ret["inherits_transform"] = inherits_transform;
    ret["visible"] = visible;
    if( lua_script ) ret["lua"] = lua_script->get_path();

    for( auto c : attached_components) ret["components"][c->get_component_name()] = c->to_yaml_node();

    std::string ref_path = SaucerEditor::get_reference_path(this);
    if( ref_path.empty() ){
        for( auto c : children_nodes ) ret["children"].push_back(c->to_yaml_node());
    } else {
        ret["path"] = ref_path;
    }
    return ret;
}
void        SceneNode::from_yaml_node( YamlNode yaml_node ) {
    SAUCER_ASSERT( children_nodes.size()==0 , "A SceneNode when instantied from YamlNode should not have any children." );
    SAUCER_ASSERT( lua_script==nullptr , "A SceneNode when instantied from YamlNode should not have a lua script attached." );
    SAUCER_ASSERT( scene==nullptr , "A SceneNode when instantied from YamlNode should not be inside a scene." );

    bool referenced = yaml_node["path"].IsDefined();
    
    if( yaml_node["components"].IsDefined() && attached_components.size() == 0 )
    for( auto c : yaml_node["components"] ){
        std::string component_name = c.first.as<std::string>();
        void(SceneNode::*f)() = __component_constructors[component_name] ;
        (this->*f)();
        attached_components.back()->from_yaml_node(c.second);
    }

    if( referenced ){
        std::string node_template_path = yaml_node["path"].as<std::string>();
        NodeTemplateResource* node_template = ResourceManager::get_resource<NodeTemplateResource>( node_template_path );
        from_yaml_node( node_template->get_yaml_node() );
        SaucerEditor::flag_as_referenced(this,node_template_path);
    }
    
    if( yaml_node["name"].IsDefined())               set_name               ( yaml_node["name"].as<decltype(name)>()                             );
    if( yaml_node["position"].IsDefined())           set_position           ( yaml_node["position"].as<decltype(position)>()                     );
    if( yaml_node["scale"].IsDefined())              set_scale              ( yaml_node["scale"].as<decltype(scale)>()                           );
    if( yaml_node["rotation_degrees"].IsDefined())   set_rotation_degrees   ( yaml_node["rotation_degrees"].as<float>()                          );
    if( yaml_node["z"].IsDefined())                  set_z                  ( yaml_node["z"].as<decltype(z)>()                                   );
    if( yaml_node["relative_z"].IsDefined())         set_relative_z         ( yaml_node["relative_z"].as<decltype(relative_z)>()                 );
    if( yaml_node["modulate"].IsDefined())           set_modulate           ( yaml_node["modulate"].as<decltype(modulate)>()                     );
    if( yaml_node["self_modulate"].IsDefined())      set_self_modulate      ( yaml_node["self_modulate"].as<decltype(self_modulate)>()           );
    if( yaml_node["inherits_transform"].IsDefined()) set_inherits_transform ( yaml_node["inherits_transform"].as<decltype(inherits_transform)>() );
    if( yaml_node["visible"].IsDefined())            set_visible            ( yaml_node["visible"].as<decltype(visible)>()                       );
    if( yaml_node["lua"].IsDefined() )               set_script( ResourceManager::get_resource<LuaScriptResource>(yaml_node["lua"].as<std::string>())); 
    

    if( !referenced && yaml_node["children"].IsDefined() )
    for( auto c : yaml_node["children"] ){
        SceneNode* new_child = new SceneNode();
        new_child->from_yaml_node( c );
        add_child(new_child);
    }
    
}
