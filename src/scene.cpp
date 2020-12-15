#include "scene.h"

#include "scene_node.h"
#include <queue>

Scene::Scene(){
    root_node = NULL;
    current_camera = NULL;
}
Scene::~Scene(){
    if(root_node) delete root_node;
}
        
void    Scene::set_root_node(SceneNode* p_root_node){
    root_node = p_root_node;
    root_node->scene = this;
}


void    Scene::propagate_input_event( Input::InputEvent* input_event ){
    if(root_node){
        std::queue<SceneNode*> input_processors;
        input_processors.push(root_node);
        // Cascading down
        while( !input_processors.empty() && !input_event->input_event_key.is_solved ){
            SceneNode* next = input_processors.front();
            input_processors.pop();
            next->process_input_event( input_event );
            for( auto child : next->get_children() ){
                input_processors.push(child);
            }
        }
    }
}