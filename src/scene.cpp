#include "scene.h"
#include "core.h"
#include <queue>
#include <algorithm>


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
    current_rendering_datas.clear();
    current_input_handlers.clear();
    current_physics_actors.clear();
    current_script_actors.clear();
    
    std::queue< AccumulatedTreeNode > nodes_queue;
    
    if( root_node ){
        AccumulatedTreeNode tree_node;
        tree_node.n = root_node;
        tree_node.t = root_node->get_transform();
        tree_node.c = root_node->get_modulate();
        nodes_queue.push(tree_node);
    }

    while( nodes_queue.size() ){
        AccumulatedTreeNode tree_node = nodes_queue.front();
        SceneNode* scene_node = tree_node.n;
        
        auto render_object_range = RenderObject::recover_range_from_node( scene_node );
        for( auto it = render_object_range.first ; it != render_object_range.second ; it++ ){
            auto render_data = it->second->get_render_data();
            render_data.model_transform = tree_node.t;
            render_data.final_modulate = tree_node.c * scene_node->get_self_modulate();
            current_rendering_datas.push_back( render_data );
        }
        
        if( scene_node->get_component<CollisionBody>() )
            current_physics_actors.push_back( scene_node );
        if( scene_node->get_script() )
            current_script_actors.push_back( scene_node );
        nodes_queue.pop();
        
        
        for( auto child : scene_node->get_children() ){
            AccumulatedTreeNode child_tree_node;
            child_tree_node.n = child;
            child_tree_node.t = (child->get_inherits_transform()) ? (tree_node.t*child->get_transform() ):(child->get_transform());
            child_tree_node.c = tree_node.c * child->get_modulate();
            nodes_queue.push( child_tree_node );
        }

    }
}
void            Scene::loop_draw(){
    
    Transform camera_transf;
    Camera* camera = get_current_camera();
    if( camera ){
        camera_transf = get_current_camera()->get_node()->get_global_transform();
        camera_transf.scale( camera->get_zoom() );
        camera_transf = camera_transf.inverted();
    }

    // Z-sorting. Must be stable_sort or else unexpected "z flips" can occur between same z-level sprites sometimes...
    std::stable_sort( current_rendering_datas.begin() , current_rendering_datas.end() , []( const RenderData& data1 , const RenderData& data2 )->bool{
        return data2.model_transform.m[10] > data1.model_transform.m[10];
    });
    Engine::get_render_engine()->set_camera_transform( camera_transf );
    Engine::get_render_engine()->update( current_rendering_datas );

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
    loop_draw();
    loop_input();
    loop_physics();
    loop_script();
}
void            Scene::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( Scene , get_root_node );
    REGISTER_LUA_MEMBER_FUNCTION( Scene , set_root_node );
}