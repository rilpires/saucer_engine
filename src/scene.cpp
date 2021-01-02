#include "scene.h"
#include "core.h"

#include <queue>
#include <algorithm>

template<> void LuaEngine::push( lua_State* ls , Scene* r ){
    lua_pushnumber(ls,666);
}

Scene::Scene(){
    root_node = NULL;
}
Scene::~Scene(){
    if(root_node) delete root_node;
    if( Engine::get_current_scene()==this ) Engine::set_current_scene(nullptr);
}
Scene*      Scene::lua_new(){ return new Scene(); }
void        Scene::set_root_node(SceneNode* p_root_node){
    root_node = p_root_node;
    root_node->scene = this;
}
SceneNode*  Scene::get_root_node(){return root_node;}
Transform   Scene::get_camera_transform(){return camera_transform;}
void        Scene::set_camera_transform(Transform t){camera_transform=t;}
void        Scene::update_current_actors(){
    current_draws.clear();
    current_input_handlers.clear();
    current_physics_actors.clear();
    current_script_actors.clear();
    
    std::queue<SceneNode*> nodes_queue;
    nodes_queue.push(get_root_node());
    
    while( nodes_queue.size() ){
        SceneNode* scene_node = nodes_queue.front();
        if( scene_node->get_component<Sprite>() ) 
            current_draws.push_back( scene_node );
        if( scene_node->get_script_resource() )
            current_script_actors.push_back( scene_node );
        nodes_queue.pop();
        
        
        for( auto child : scene_node->get_children() )
            nodes_queue.push( child );

    }
}
void        Scene::loop_draw(){

    GL_CALL( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
    GL_CALL( glClearColor( 0,0,0,1 ) );
    
    Vector2 window_size = Engine::get_window_size();
    
    unsigned int program_location = 1U;
    GL_CALL( unsigned int viewport_size_attrib_location = glGetUniformLocation(program_location,"viewport_size") );
    GL_CALL( unsigned int model_transf_attrib_location  = glGetUniformLocation(program_location,"model_transf") );
    GL_CALL( unsigned int camera_transf_attrib_location = glGetUniformLocation(program_location,"camera_transf") );
    
    GL_CALL( glUniform2f( viewport_size_attrib_location , window_size.x , window_size.y ) );
    GL_CALL( glUniformMatrix4fv( camera_transf_attrib_location , 1 , GL_FALSE , camera_transform.m ) );

    
    
    // Z-sorting
    std::sort( current_draws.begin() , current_draws.end() , [](const SceneNode* n1 , const SceneNode* n2)->bool{
        return n2->get_global_z() > n1->get_global_z();
    });

    for( auto it = current_draws.begin() ; it != current_draws.end() ; it ++ ){
        SceneNode* scene_node = *it;
        Transform model_transform = scene_node->get_global_transform();
        model_transform.scale(Vector3(1,1,-1));

        GL_CALL( glBindTexture( GL_TEXTURE_2D , scene_node->get_component<Sprite>()->get_texture()->get_texture_id() ) );
        GL_CALL( glUniformMatrix4fv( model_transf_attrib_location , 1 , GL_FALSE , model_transform.m ) );
        GL_CALL( glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,NULL) );
    }
}
void        Scene::loop_input(){
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
void        Scene::loop_script(){
    for( auto it = current_script_actors.begin() ; it != current_script_actors.end() ; it++ ){
        LuaEngine::execute_frame_start( *it , Engine::get_last_frame_duration() );
    }
}
void        Scene::loop_physics(){

}
void        Scene::loop(){
    update_current_actors();
    loop_input();
    loop_physics();
    loop_script();
    loop_draw();
}
void        Scene::bind_methods(){

}