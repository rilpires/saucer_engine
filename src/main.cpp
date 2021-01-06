#include "core.h"

int main( int argc , char** argv ){
    for( int i = 0 ; i < argc ; i++ )
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;  
    

    Engine::initialize();
    Engine::set_window_size( Vector2(640,480) );
    Engine::set_window_title("SaucerEngine");
    LuaEngine::initialize();

    Scene* scene = new Scene();
    SceneNode* root = new SceneNode();
    Engine::set_current_scene( scene );
    root->set_script( LuaScriptResource::get_resource("res/scripts/test.lua") );
    scene->set_root_node(root);

    ResourceManager::get_resource("res/shaders/basic.glsl");
    
    while( !Engine::should_close() ) 
    Engine::update(); 
    Engine::close();
    
    LuaEngine::finish();
    std::cout << "Bye bye!!!" << std::endl;
}