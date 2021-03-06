#include "engine.h"
#include "core.h"


// How many last frames should be accounted to calculate frames per second
#define FPS_FRAMES_TO_ACCOUNT 30

RenderEngine*       Engine::render_engine   = nullptr;
AudioEngine*        Engine::audio_engine    = nullptr;
Scene*              Engine::current_scene   = nullptr;
double              Engine::next_frame_time   = 0;
std::list<double>   Engine::last_uptimes;
bool                Engine::exiting = false;
ProjectConfig*      Engine::project_config = nullptr;

void            Engine::initialize( std::string config_path ){
    ResourceManager::fetch_package();
    
    project_config = ResourceManager::get_resource<ProjectConfig>(config_path);
    if( !project_config ){
        project_config = new ProjectConfig();
        ResourceManager::set_resource("res/project.config",project_config);
        project_config->save_as_file("res/project.config");
    }

    render_engine = new RenderEngine( project_config );
    audio_engine = new AudioEngine();
    LuaEngine::initialize();
    
    current_scene = NULL;
    while(last_uptimes.size()<FPS_FRAMES_TO_ACCOUNT)
        last_uptimes.push_back(-1);

    
    // Setting input callbacks
    glfwSetCursorPosCallback( render_engine->get_glfw_window() , Input::mouse_pos_callback );
    glfwSetKeyCallback( render_engine->get_glfw_window() , Input::key_callback );
    glfwSetMouseButtonCallback( render_engine->get_glfw_window() , Input::mouse_button_callback );
    glfwSetCharCallback( render_engine->get_glfw_window() , Input::char_callback );
    glfwSetWindowSizeCallback( render_engine->get_glfw_window(), RenderEngine::__window_resize_callback );

    if( is_editor() ) SaucerEditor::setup();
    
    Scene* scene = new Scene();
    Engine::set_current_scene( scene );
    NodeTemplateResource* root_template = ResourceManager::get_resource<NodeTemplateResource>( project_config->get_initial_root_path() );
    SceneNode* root;
    if( root_template ){
        root = root_template->instantiate_node();
        #ifdef SAUCER_EDITOR
        SaucerEditor::reference_path[root->get_saucer_id()] = project_config->get_initial_root_path();
        #endif
    }
    else{
        root = new SceneNode();
    } 
    scene->set_root_node(root);
    
}
void            Engine::close(){
    if(current_scene) delete current_scene;
    delete render_engine;
    delete audio_engine;
}
void            Engine::update(){
    double remaining_time = next_frame_time - get_uptime(); 
    if (remaining_time > 0) Engine::u_sleep(remaining_time * 1000000);
    next_frame_time = get_uptime() + 1.0/60.0;
    glfwPollEvents();
    last_uptimes.push_front( get_uptime() ); 
    last_uptimes.pop_back();
    if (current_scene) current_scene->loop();
    
    #ifdef SAUCER_EDITOR 
        SaucerEditor::update();
    #endif

    glfwSwapBuffers( render_engine->get_glfw_window() );

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
        double ret = *(begin) - *(++begin);
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
Vector2         Engine::get_viewport_size(){
    return render_engine->get_viewport_rect().get_size();
}
void            Engine::set_fullscreen( bool fs ){
    render_engine->set_fullscreen(fs);
}
bool            Engine::is_fullscreen(){
    return render_engine->is_fullscreen();
}
std::string     Engine::get_window_title(){
    return render_engine->get_window_title();
}
void            Engine::set_window_title( std::string new_title ){
    render_engine->set_window_title(new_title);
}
RenderEngine*   Engine::get_render_engine(){
    return render_engine;
}
AudioEngine*    Engine::get_audio_engine(){
    return audio_engine;
}
bool            Engine::should_close() {
    return exiting || ( render_engine && render_engine->should_close() );
}
void             Engine::exit(){
    if( current_scene && current_scene->get_root_node() ) current_scene->get_root_node()->queue_free();
    exiting = true;
}
ProjectConfig*  Engine::get_config(){
    return project_config;
}
void            Engine::bind_methods(){
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_uptime );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_fps );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_current_scene );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_current_scene );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_window_size );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_window_size );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_viewport_size );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_window_pos );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_window_pos );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_fullscreen );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , is_fullscreen );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_render_engine );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_audio_engine );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , exit );

}
#ifdef SAUCER_EDITOR
bool            Engine::is_playing(){ return SaucerEditor::currently_playing; };
#endif

#ifdef _WIN32
void            Engine::u_sleep( long long int usec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(usec/1000));
}
#else
void            Engine::u_sleep( long long int usec)
{
    usleep(usec);
}
#endif
