#include "core.h"

int main( int argc , char** argv ){
    for( int i = 0 ; i < argc ; i++ )
        saucer_print( "argv[" , i , "] = " , argv[i] );  
    

    Engine::initialize();
    Engine::set_window_size( Vector2(640,480) );
    Engine::set_window_title("SaucerEngine");
    LuaEngine::initialize();

    auto cursor_texture = (TextureResource*)ResourceManager::get_resource("res/cursor.png");
    if( cursor_texture ) Engine::get_render_engine()->set_custom_cursor(cursor_texture,4,1);

    Scene* scene = new Scene();
    SceneNode* root = new SceneNode();
    Engine::set_current_scene( scene );
    root->set_script( LuaScriptResource::get_resource("res/scripts/root.lua") );
    scene->set_root_node(root);

    // for( int i = 0 ; i < 50000 ; i++ ){
    //     SceneNode* new_node = new SceneNode();
    //     root->add_child( new_node );
    //     new_node->create_component<Sprite>()->set_texture( (TextureResource*)ResourceManager::get_resource("res/gold.png") );
    //     new_node->set_position( Vector2( (float(random()%1000))-500 , (float(random()%1000))-500  ));
    // }

    while( !Engine::should_close() )
        Engine::update(); 
    Engine::close();
    
    LuaEngine::finish();
    saucer_print( "Bye bye!!!" );
}