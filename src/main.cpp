#include "core.h"

int main( int argc , char** argv ){
    for( int i = 0 ; i < argc ; i++ )
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;  

    Engine::initialize( Vector2(640,480) , Vector2(100,100) , "SaucerEngine (GLFW+OpenGL)" );
    
    LuaEngine::initialize();

    Scene* scene = new Scene();
    SceneNode* root = new SceneNode();
    Engine::set_current_scene( scene );
    root->set_script( LuaScriptResource::get_resource("res/scripts/test.lua") );
    scene->set_root_node(root);

    
    while( !Engine::should_close() ){
        Engine::update();
    }

    LuaEngine::finish();
    std::cout << "Bye bye!!!" << std::endl;
}