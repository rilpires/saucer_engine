#include "scene.h"
#include "core.h"
#include <queue>
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
    std::queue< AccumulatedTreeNode > nodes_queue;
    std::vector< AccumulatedTreeNode > render_objs;
    std::vector< RenderData > render_datas;
    if( root_node && root_node->is_visible() ){
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
            if( child->is_visible() ){
                AccumulatedTreeNode child_tree_node;
                child_tree_node.n = child;
                child_tree_node.t = (child->get_inherits_transform()) ? (tree_node.t*child->get_transform() ):(child->get_transform());
                child_tree_node.c = tree_node.c * child->get_modulate();
                nodes_queue.push( child_tree_node );
            }
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
void            Scene::loop_input(){
    
    std::queue< SceneNode* > nodes_queue;
    std::vector< SceneNode* > script_actors;
    Vector2 world_mouse_pos = Input::get_world_mouse_position();
    AnchoredRect* next_hovered = nullptr;

    // Traversing the tree looking for the hovered AnchoredRect  
    if( root_node && root_node->is_visible() )
        nodes_queue.push(root_node);
    while( nodes_queue.size() ){
        SceneNode* scene_node = nodes_queue.front();
        auto anchored_rects = AnchoredRect::recover_range_from_node(scene_node);
        for( auto& it = anchored_rects.first ; it != anchored_rects.second ; it++ ){
            AnchoredRect* rect = it->second;
            if( rect->ignore_mouse ) continue;
            Transform t = rect->get_parent_global_transform();
            Vector2 rect_size       = rect->get_rect_size();
            Vector2 rect_pos        = rect->get_rect_pos();
            Vector2 world_top_left        = t*rect_pos;
            Vector2 world_bottom_right    = t*(rect_pos+rect_size); // I hope it isn't rotated! won't work
            if( world_mouse_pos.x > world_top_left.x 
            &&  world_mouse_pos.x < world_bottom_right.x 
            &&  world_mouse_pos.y > world_top_left.y 
            &&  world_mouse_pos.y < world_bottom_right.y  ){
                next_hovered = rect;
            }
        }
        for( auto child : scene_node->get_children() )
            if( child->is_visible() )
                nodes_queue.push( child );
        nodes_queue.pop();
    }
    set_current_hovered_anchored_rect(next_hovered);
    
    // Traversing the tree solving input.
    // Note we make a new traversing for each input event
    Input::InputEvent* next_input_event = Input::pop_event_queue();
    while(next_input_event){

        // We may have a new focused rect:
        if( next_input_event->get_type() == INPUT_EVENT_TYPE::MOUSE_BUTTON 
        && next_input_event->is_pressed() 
        && next_input_event->get_button() == INPUT_EVENT_MOUSE_BUTTON::BUTTON_LEFT ){
            set_current_focused_anchored_rect( current_hovered_anchored_rect );
        }

        // Checking cb_mouse_button/key/char of the focused rect
        if( current_focused_anchored_rect ){
            switch (next_input_event->get_type())
            {
                case INPUT_EVENT_TYPE::MOUSE_BUTTON:
                    current_focused_anchored_rect->cb_mouse_button(next_input_event->input_event_mouse_button);
                    break;
                case INPUT_EVENT_TYPE::KEY:
                    current_focused_anchored_rect->cb_key(next_input_event->input_event_key);
                    break;
                case INPUT_EVENT_TYPE::CHAR:
                    current_focused_anchored_rect->cb_char(next_input_event->input_event_char);
                    break;
                default: break;
            }
        }
        
        if( root_node && !(next_input_event->is_solved()) )
            nodes_queue.push(root_node);
        while( nodes_queue.size() ){
            SceneNode* scene_node = nodes_queue.front();
            auto anchored_rects = AnchoredRect::recover_range_from_node(scene_node);

            if( scene_node->get_script() )
                LuaEngine::execute_callback( "input" , scene_node , next_input_event );
            
            if( next_input_event->is_solved() ) {
                while( nodes_queue.size() ) 
                    nodes_queue.pop();
            } 
            else {
                for( auto child : scene_node->get_children() )
                    nodes_queue.push( child );
                nodes_queue.pop();
            }
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
            LuaEngine::execute_callback("frame_start", scene_node , last_frame_duration );
        for( auto child : scene_node->get_children() )
            nodes_queue.push( child );
        nodes_queue.pop();
    }

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
void            Scene::queue_free_node(SceneNode* node){
    to_free.insert(node);
}
void            Scene::loop(){
    loop_input();
    for( auto& it : to_free ){ delete it; } to_free.clear();
    loop_script();
    for( auto& it : to_free ){ delete it; } to_free.clear();
    loop_physics();
    for( auto& it : to_free ){ delete it; } to_free.clear();
    loop_draw();
}
void            Scene::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( Scene , get_root_node );
    REGISTER_LUA_MEMBER_FUNCTION( Scene , set_root_node );
}