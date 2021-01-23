#define GLEW_STATIC

#include "core.h"



int main( int argc , char** argv ){
    for( int i = 0 ; i < argc ; i++ )
        saucer_print( "argv[" , i , "] = " , argv[i] );  
    
    Engine::initialize();
    Engine::set_window_title("SaucerEngine");
    LuaEngine::initialize();

    auto cursor_texture = (TextureResource*)ResourceManager::get_resource("res/cursor.png");
    if( cursor_texture ) Engine::get_render_engine()->set_custom_cursor(cursor_texture,4,1);

    
    Scene* scene = new Scene();
    SceneNode* root = new SceneNode();
    root->from_yaml_node( YAML::LoadFile("config.yaml") );
    Engine::set_current_scene( scene );
    scene->set_root_node(root);

    // Saving
    // YAML::Node n = root->to_yaml_node();
    // std::ofstream fout("config.yaml");
    // fout << n;
    // fout.close();

    while( !Engine::should_close() )
        Engine::update();
    Engine::close();
    
    LuaEngine::finish();
    saucer_print( "Bye bye!!!" );
}