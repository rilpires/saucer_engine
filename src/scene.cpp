#include "scene.h"
#include "core.h"
#include <queue>
#include <stack>
#include <algorithm>


Scene::Scene(){
    root_node = NULL;
    current_hovered_anchored_rect = nullptr;
    current_focused_anchored_rect = nullptr;
    current_camera = nullptr;
    collision_world = new CollisionWorld();
}
Scene::~Scene(){
    if(root_node) delete root_node;
    if( Engine::get_current_scene()==this ) Engine::set_current_scene(nullptr);
    delete collision_world;
}
Scene*          Scene::lua_new(){ return new Scene(); }
void            Scene::set_root_node(SceneNode* new_root_node){
    SceneNode* old_root_node = root_node;
    root_node = new_root_node;
    if ( old_root_node ){
        old_root_node->get_out();
    }
    if( root_node ){
        root_node->set_scene(this);
        root_node->entered_tree();
    }
}
SceneNode*      Scene::get_root_node(){
    return root_node;
}
void            Scene::set_current_camera( Camera* new_camera ){
    current_camera = new_camera;
}
AnchoredRect*   Scene::get_current_hovered_anchored_rect() const{
    return current_hovered_anchored_rect;
}
AnchoredRect*   Scene::get_current_focused_anchored_rect() const{
    return current_focused_anchored_rect;
}
Camera*         Scene::get_current_camera() const {
    return current_camera;
}
CollisionWorld* Scene::get_collision_world() const{
    return collision_world;
}
void            Scene::loop_draw(){
    
    // Traversing the whole tree filling render_datas in proper order
    std::stack< AccumulatedTreeNode > node_stack;
    std::vector< AccumulatedTreeNode > render_objs;
    std::vector< RenderData > render_datas;
    if( root_node && root_node->get_visible() ){
        AccumulatedTreeNode tree_node;
        tree_node.n = root_node;
        tree_node.t = root_node->get_transform();
        tree_node.c = root_node->get_modulate();
        node_stack.push(tree_node);
    }
    while( node_stack.size() ){
        AccumulatedTreeNode tree_node = node_stack.top();
        SceneNode* scene_node = tree_node.n;
        render_objs.push_back(tree_node);
        node_stack.pop();

        auto node_children = scene_node->get_children();
        for( auto it = node_children.rbegin() ; it != node_children.rend() ; it++ ){
            SceneNode* child = *it;
            if( child->get_visible() ){
                AccumulatedTreeNode child_tree_node;
                Transform child_transf = child->get_transform();
                Transform& parent_transf = tree_node.t;
                child_tree_node.n = child;
                child_tree_node.c = tree_node.c * child->get_modulate();
                if( child->get_relative_z() ){
                    if(child->get_inherits_transform()){
                        child_tree_node.t = parent_transf * child_transf;
                    } else {
                        child_tree_node.t = child_transf;
                        child_tree_node.t.m[11] = parent_transf.m[11] + child_transf.m[11];
                    }
                } else {
                    if(child->get_inherits_transform()){
                        child_tree_node.t = parent_transf * child_transf;
                        child_tree_node.t.m[11] = child_transf.m[11];
                    } else {
                        child_tree_node.t = child_transf;
                    }
                }
                node_stack.push( child_tree_node );
            }
        }
    }

    // Z-sorting. Must be stable_sort or else unexpected "z flips" can occur between same z-level sprites sometimes...
    std::stable_sort( render_objs.begin() , render_objs.end() , []( const AccumulatedTreeNode& data1 , const AccumulatedTreeNode& data2 )->bool{
        return data2.t.m[11] > data1.t.m[11];
    });
    
    // Expanding render_objects into render_data
    std::for_each( render_objs.begin() , render_objs.end() , [&]( const AccumulatedTreeNode& tree_node ){
        auto render_object_range = RenderObject::recover_range_from_node( tree_node.n );
        for( auto it = render_object_range.first ; it != render_object_range.second ; it++ ){
            std::vector<RenderData> v = it->second->generate_render_data();
            for( RenderData& render_data : v ){
                if(render_data.use_tree_transform) render_data.model_transform = tree_node.t;
                render_data.final_modulate = tree_node.c * tree_node.n->get_self_modulate();
                // render_data.fill_vertices_modulate(); // This is actually insane right? I'll be using uniforms...
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
void            Scene::loop_audio(){
    for( auto p : StreamPlayer::component_from_node() ){
        StreamPlayer* streamer = p.second;
        streamer->update_stream();
    }
}
void            Scene::loop_input(){
    if( !Engine::is_playing() ){ while( Input::pop_event_queue() ){} return; }
    
    std::stack< SceneNode* > node_stack;
    std::vector< AnchoredRect* > anchored_rects;
    std::vector< SceneNode* > script_actors;
    Vector2 world_mouse_pos = Input::get_world_mouse_position();
    AnchoredRect* next_hovered = nullptr;
    int next_hovered_z = (short)(1<<15);

    // Traversing the tree looking for the hovered AnchoredRect  
    if( root_node && root_node->get_visible() )
        node_stack.push(root_node);
    while( node_stack.size() ){
        SceneNode* scene_node = node_stack.top();
        node_stack.pop();
        auto children = scene_node->get_children();
        for( auto it = children.rbegin() ; it != children.rend() ; it++ ){
            SceneNode* child = *it;
            if( child->get_visible() )
                node_stack.push( child );
        }
        auto attached_anchored_rects = AnchoredRect::recover_range_from_node(scene_node);
        for( auto& it = attached_anchored_rects.first ; it != attached_anchored_rects.second ; it++ )
            if( it->second->ignore_mouse == false )
                anchored_rects.push_back(it->second);
    }
    for( auto rect_it = anchored_rects.begin() ; rect_it != anchored_rects.end() ; rect_it++ ){
        AnchoredRect* rect = *rect_it;
        Transform t = rect->get_node()->get_global_transform();
        Vector2 world_top_left     = t * ( rect->get_offset() );
        Vector2 world_bottom_right = t * ( rect->get_rect_size() + rect->get_offset() ); // I hope it isn't rotated! won't work
        if( world_mouse_pos.x > world_top_left.x 
        &&  world_mouse_pos.x < world_bottom_right.x 
        &&  world_mouse_pos.y > world_top_left.y 
        &&  world_mouse_pos.y < world_bottom_right.y
        &&  rect->get_node()->get_global_z() >= next_hovered_z ){
            next_hovered = rect;
            next_hovered_z = rect->get_node()->get_global_z();
        }
    }
    set_current_hovered_anchored_rect(next_hovered);
    
    // Traversing the tree solving input.
    // Note we make a new traversing for each input event
    Input::InputEvent* next_input_event = Input::pop_event_queue();
    while(next_input_event){

        // We may have a new focused rect:
        if( next_input_event->get_type() == INPUT_EVENT_TYPE_MOUSE_BUTTON 
        && next_input_event->is_pressed() 
        && next_input_event->get_button() == INPUT_EVENT_MOUSE_BUTTON::BUTTON_LEFT ){
            set_current_focused_anchored_rect( current_hovered_anchored_rect );
        }

        // Checking cb_mouse_button/key/char of the focused rect
        if( current_focused_anchored_rect ){
            switch (next_input_event->get_type())
            {
                case INPUT_EVENT_TYPE_MOUSE_BUTTON:
                    current_focused_anchored_rect->cb_mouse_button(next_input_event->input_event_mouse_button);
                    break;
                case INPUT_EVENT_TYPE_KEY:
                    current_focused_anchored_rect->cb_key(next_input_event->input_event_key);
                    break;
                case INPUT_EVENT_TYPE_CHAR:
                    current_focused_anchored_rect->cb_char(next_input_event->input_event_char);
                    break;
                default: break;
            }
        }
        
        if( root_node && !(next_input_event->is_solved()) )
            node_stack.push(root_node);
        while( node_stack.size() ){
            SceneNode* scene_node = node_stack.top();
            node_stack.pop();
            for( auto child : scene_node->get_children() )
                node_stack.push( child );

            LuaEngine::execute_callback( "input" , scene_node , next_input_event );
            
            if( next_input_event->is_solved() ) 
                while( node_stack.size() ) 
                    node_stack.pop();
        }

        delete next_input_event;   
        next_input_event = Input::pop_event_queue();
    }

}
void            Scene::loop_script(){
    if( !Engine::is_playing() ) return;
    std::stack< SceneNode* > node_stack;
    std::vector< SceneNode* > actors;
    double last_frame_duration = Engine::get_last_frame_duration();

    // Discovering tree DFS
    if( root_node )
        node_stack.push(root_node);
    while( node_stack.size() ){
        SceneNode* scene_node = node_stack.top();
        node_stack.pop();
        if( LuaEngine::has_actor_table(scene_node) ) actors.push_back(scene_node);
        for( auto child : scene_node->get_children() )
            node_stack.push( child );
    }
    
    // Calling frame_start in reverse tree DFS discovering order (root will be called last)
    for( auto actor_it = actors.rbegin() ; actor_it != actors.rend() ; actor_it++ )
        LuaEngine::execute_callback("frame_start", *actor_it , last_frame_duration );

}
void            Scene::set_current_hovered_anchored_rect( AnchoredRect* r ){
    if( current_hovered_anchored_rect == r ) return;
    if( current_hovered_anchored_rect ){
        current_hovered_anchored_rect->cb_mouse_exiting();
        LuaEngine::execute_callback("exiting_mouse",current_hovered_anchored_rect->get_node()); 
    }
    current_hovered_anchored_rect = r;
    if( current_hovered_anchored_rect ){
        current_hovered_anchored_rect->cb_mouse_entered();
        LuaEngine::execute_callback("entered_mouse",current_hovered_anchored_rect->get_node()); 
    }
}
void            Scene::set_current_focused_anchored_rect( AnchoredRect* r ){
    current_focused_anchored_rect = r;
}
void            Scene::loop_physics(){
    if( !Engine::is_playing() ) return;
    collision_world->step();
    
    for( auto it : CollisionBody::component_from_node() ){
        CollisionBody*& body = it.second;
        SceneNode* node = body->get_node();
        if( node->get_scene() ){
            node->set_position( body->get_position() );
            node->set_rotation_degrees( body->get_rotation_degrees() );
        }
    }
    collision_world->delete_disableds();   
}
void            Scene::queue_free_node(SceneNode* node){
    to_free.insert(node);
}
void            Scene::loop(){
    for( auto& it : to_free ){ delete it; } to_free.clear();
    loop_input();
    for( auto& it : to_free ){ delete it; } to_free.clear();
    loop_script();
    for( auto& it : to_free ){ delete it; } to_free.clear();
    loop_physics();
    for( auto& it : to_free ){ delete it; } to_free.clear();
    loop_audio();
    loop_draw();
}
void            Scene::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( Scene , get_root_node );
    REGISTER_LUA_MEMBER_FUNCTION( Scene , set_root_node );
    REGISTER_LUA_MEMBER_FUNCTION( Scene , get_current_hovered_anchored_rect );
    REGISTER_LUA_MEMBER_FUNCTION( Scene , get_current_focused_anchored_rect );
}