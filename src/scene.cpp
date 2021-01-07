#include "scene.h"
#include "core.h"
#include <queue>


Scene::Scene(){
    root_node = NULL;
    collision_world = new CollisionWorld();
}
Scene::~Scene(){
    if(root_node) delete root_node;
    if( Engine::get_current_scene()==this ) Engine::set_current_scene(nullptr);
    delete collision_world;
}
Scene*          Scene::lua_new(){ return new Scene(); }
void            Scene::set_root_node(SceneNode* new_root_node){
    if( !root_node && new_root_node ){
        root_node = new_root_node;
        root_node->set_scene(this);
        root_node->entered_tree();
    }
    else if ( root_node && !new_root_node ){
        SceneNode* temp = root_node;
        root_node = nullptr;
        temp->get_out();
    }
}
SceneNode*      Scene::get_root_node(){
    return root_node;
}
void            Scene::set_current_camera( Camera* new_camera ){
    current_camera = new_camera;
}
Camera*         Scene::get_current_camera() const {
    return current_camera;
}
CollisionWorld* Scene::get_collision_world() const{
    return collision_world;
}
void            Scene::update_current_actors(){
    current_input_handlers.clear();
    current_physics_actors.clear();
    current_script_actors.clear();
    
    std::queue<SceneNode*> nodes_queue;
    nodes_queue.push(get_root_node());
    
    while( nodes_queue.size() ){
        SceneNode* scene_node = nodes_queue.front();
        if( scene_node->get_component<CollisionBody>() )
            current_physics_actors.push_back( scene_node );
        if( scene_node->get_script() )
            current_script_actors.push_back( scene_node );
        nodes_queue.pop();
        
        
        for( auto child : scene_node->get_children() )
            nodes_queue.push( child );

    }
}
void            Scene::loop_draw(){

    std::vector<RenderObject*> all_render_objects;
    
    for( auto it = RenderObject::component_from_node.begin() ; it != RenderObject::component_from_node.end() ; it++ )
        if( it->second->get_node()->get_scene() == this )
            all_render_objects.push_back( it->second );

    Transform camera_transf;
    Camera* camera = get_current_camera();
    if( camera ){
        camera_transf = get_current_camera()->get_node()->get_global_transform();
        camera_transf.scale( camera->get_zoom() );
        camera_transf = camera_transf.inverted();
    }
    Engine::get_render_engine()->set_camera_transform( camera_transf );
    Engine::get_render_engine()->update( all_render_objects );

}
void            Scene::loop_input(){
    Input::InputEvent* next_input_event = Input::pop_event_queue();
    while(next_input_event){
        for( auto it = current_script_actors.begin() ; 
             it != current_script_actors.end() && !next_input_event->is_solved() ;
             it++ )
        {
            SceneNode* node_actor = *it;
            LuaEngine::execute_input( node_actor , next_input_event );
        }
        delete next_input_event;   
        next_input_event = Input::pop_event_queue();
    }
}
void            Scene::loop_script(){
    for( auto it = current_script_actors.begin() ; it != current_script_actors.end() ; it++ ){
        LuaEngine::execute_frame_start( *it , Engine::get_last_frame_duration() );
    }
}
void            Scene::loop_physics(){
    collision_world->step();
    for( SceneNode*& node : current_physics_actors ){
        if( node->get_scene() ){
            CollisionBody* body = node->get_component<CollisionBody>();
            if( body ){
                node->set_position( body->get_position() );
                node->set_rotation_degrees( body->get_rotation_degrees() );
            }
        }
    }
    collision_world->delete_disableds();   
}
void            Scene::loop(){
    update_current_actors();
    loop_input();
    loop_physics();
    loop_script();
    loop_draw();
}
void            Scene::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( Scene , get_root_node );
    REGISTER_LUA_MEMBER_FUNCTION( Scene , set_root_node );
}