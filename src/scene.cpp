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
void            Scene::loop_draw(){
    
    // Traversing the whole tree filling render_datas in proper order
    std::queue< AccumulatedTreeNode > nodes_queue;
    std::vector< AccumulatedTreeNode > render_objs;
    std::vector< RenderData > render_datas;
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
        render_objs.push_back(tree_node);
        for( auto child : scene_node->get_children() ){
            AccumulatedTreeNode child_tree_node;
            child_tree_node.n = child;
            child_tree_node.t = (child->get_inherits_transform()) ? (tree_node.t*child->get_transform() ):(child->get_transform());
            child_tree_node.c = tree_node.c * child->get_modulate();
            nodes_queue.push( child_tree_node );
        }
        nodes_queue.pop();
    }

    // Z-sorting. Must be stable_sort or else unexpected "z flips" can occur between same z-level sprites sometimes...
    std::stable_sort( render_objs.begin() , render_objs.end() , []( const AccumulatedTreeNode& data1 , const AccumulatedTreeNode& data2 )->bool{
        return data2.t.m[10] > data1.t.m[10];
    });
    
    // Expanding render_objects into render_data
    std::for_each( render_objs.begin() , render_objs.end() , [&]( const AccumulatedTreeNode& tree_node ){
        auto render_object_range = RenderObject::recover_range_from_node( tree_node.n );
        for( auto it = render_object_range.first ; it != render_object_range.second ; it++ ){
            std::vector<RenderData> v = it->second->generate_render_data();
            for( RenderData& render_data : v ){
                if(render_data.use_tree_transform) render_data.model_transform = tree_node.t;
                render_data.final_modulate = tree_node.c * tree_node.n->get_self_modulate();
                // render_data.fill_vertices_modulate();
                render_datas.push_back( render_data );
            }
        }
    });

    Transform camera_transf;
    Camera* camera = get_current_camera();
    if( camera ){
        camera_transf = get_current_camera()->get_node()->get_global_transform();
        camera_transf.scale( camera->get_zoom() );
    }

    Engine::get_render_engine()->set_view_transform( camera_transf.inverted() );
    Engine::get_render_engine()->update( render_datas );

}
void            Scene::loop_input(){
    std::queue< SceneNode* > nodes_queue;
    std::vector< SceneNode* > script_actors;
    
    // Traversing the tree & finding the possible actors
    if( root_node )
        nodes_queue.push(root_node);
    while( nodes_queue.size() ){
        SceneNode* scene_node = nodes_queue.front();;
        if( scene_node->get_script() )
            script_actors.push_back(scene_node);
        for( auto child : scene_node->get_children() )
            nodes_queue.push( child );
        nodes_queue.pop();
    }
    // Solving inputs
    Input::InputEvent* next_input_event = Input::pop_event_queue();
    while(next_input_event){
        for( auto it = script_actors.begin() ; 
             it != script_actors.end() && !next_input_event->is_solved() ;
             it++ )
        {
            SceneNode*& node_actor = *it;
            LuaEngine::execute_input( node_actor , next_input_event );
        }
        delete next_input_event;   
        next_input_event = Input::pop_event_queue();
    }


}
void            Scene::loop_script(){
    std::queue< SceneNode* > nodes_queue;
    double last_frame_duration = Engine::get_last_frame_duration();

    // Traversing the three while solving "_frame_start" script 
    if( root_node )
        nodes_queue.push(root_node);
    while( nodes_queue.size() ){
        SceneNode* scene_node = nodes_queue.front();;
        if( scene_node->get_script() )
            LuaEngine::execute_frame_start( scene_node , last_frame_duration );
        for( auto child : scene_node->get_children() )
            nodes_queue.push( child );
        nodes_queue.pop();
    }

}
void            Scene::loop_physics(){
    collision_world->step();
    
    for( auto it : CollisionBody::component_from_node ){
        CollisionBody*& body = it.second;
        SceneNode* node = body->get_node();
        if( node->get_scene() ){
            node->set_position( body->get_position() );
            node->set_rotation_degrees( body->get_rotation_degrees() );
        }
    }
    collision_world->delete_disableds();   
}
void            Scene::loop(){
    loop_input();
    loop_script();
    loop_physics();
    loop_draw();
}
void            Scene::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( Scene , get_root_node );
    REGISTER_LUA_MEMBER_FUNCTION( Scene , set_root_node );
}