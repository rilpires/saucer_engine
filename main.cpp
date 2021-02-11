#include "core.h"



int main( int argc , char** argv ){
    
    for( int i = 0 ; i < argc ; i++ )
        saucer_print( "argv[" , i , "] = " , argv[i] );  

    YamlNode config;
    try { 
        config = YAML::LoadFile("config.yaml"); 
    }
    catch(const YAML::BadFile::Exception& e) {
        UNUSED(e);
        saucer_warn("No config.yaml file found.");
        config["initial_window_size"] = Vector2(640,480);
    }
    
    Engine::initialize( config );
    Engine::set_window_title("SaucerEngine");

    while( !Engine::should_close() )
        Engine::update();
    Engine::close();
    
    LuaEngine::finish();
    saucer_print( "Bye bye!!!" );
}