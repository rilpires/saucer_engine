#include "engine.h"
#include "core.h"

// How many last frames should be accounted to calculate frames per second
#define FPS_FRAMES_TO_ACCOUNT 30

RenderEngine*       Engine::render_engine   = nullptr;
AudioEngine*        Engine::audio_engine    = nullptr;
Scene*              Engine::current_scene   = nullptr;
std::list<double>   Engine::last_uptimes;


void        Engine::initialize(){
    
    render_engine = new RenderEngine();
    audio_engine = new AudioEngine();
    current_scene = NULL;

    // Setting input callbacks
    glfwSetCursorPosCallback( render_engine->get_glfw_window() , Input::mouse_pos_callback );
    glfwSetKeyCallback( render_engine->get_glfw_window() , Input::key_callback );
    glfwSetMouseButtonCallback( render_engine->get_glfw_window() , Input::mouse_button_callback );

}

void        Engine::close(){
    if(current_scene) delete current_scene;
    delete render_engine;
    delete audio_engine;
}

void            Engine::update(){
    last_uptimes.push_front( get_uptime() );
    if( last_uptimes.size() > FPS_FRAMES_TO_ACCOUNT ) last_uptimes.pop_back();
    glfwPollEvents();
    if (current_scene) current_scene->loop();
}
double          Engine::get_uptime(){
    return glfwGetTime();
}
double          Engine::get_fps(){
    size_t number_of_frames = last_uptimes.size();
    double elapsed_time = last_uptimes.front() - last_uptimes.back();
    if( number_of_frames > 1 && elapsed_time > 0 ){
        return (number_of_frames-1) / elapsed_time; 
    } else return 0;
}
double          Engine::get_last_frame_duration(){
    if( last_uptimes.size() >= 2 ){
        auto begin = last_uptimes.begin();
        double ret = *(begin) - *(begin++);
        return ret;
    }
    else return 0.0;
}
void            Engine::set_current_scene(Scene* scene){
    current_scene = scene;
}
Scene*          Engine::get_current_scene(){ 
    return current_scene;
}
void            Engine::set_window_size( Vector2 new_size ){
    render_engine->set_window_size(new_size);
}
Vector2         Engine::get_window_size(){
    return render_engine->get_window_size();
}
void            Engine::set_window_pos( Vector2 new_pos ){
    render_engine->set_window_size(new_pos);
}
Vector2         Engine::get_window_pos() {
    return render_engine->get_window_pos();
}
void            Engine::set_fullscreen( bool fs ){
    render_engine->set_fullscreen(fs);
}
bool            Engine::is_fullscreen(){
    return render_engine->is_fullscreen();
}
std::string      Engine::get_window_title(){
    return render_engine->get_window_title();
}
void             Engine::set_window_title( std::string new_title ){
    render_engine->set_window_title(new_title);
}
RenderEngine*   Engine::get_render_engine(){
    return render_engine;
}
AudioEngine*    Engine::get_audio_engine(){
    return audio_engine;
}
bool            Engine::should_close() {
    return ( render_engine && render_engine->should_close() );
}
void            Engine::bind_methods(){
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_uptime );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_fps );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_current_scene );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_current_scene );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_window_size );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_window_size );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_window_pos );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_window_pos );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_fullscreen );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , is_fullscreen );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_render_engine );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_audio_engine );
}