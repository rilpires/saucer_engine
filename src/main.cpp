#include "core.h"

int main( int argc , char** argv ){
    
    for( int i = 0 ; i < argc ; i++ )
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;  


    

    Window window( Vector2(640,480) , Vector2(100,100) , "PlaygroundOpenGL" );
    
    ResourceManager::get_resource("res/gold.png");
    
    Scene* scene = new Scene();
    SceneNode* root = new SceneNode();
    SceneNode* center = new SceneNode();
    Esfera* orbit = new Esfera();
    window.set_current_scene( scene );
    scene->set_root_node(root);

    root->set_z  (-1);
    center->set_z(-2);
    orbit->set_z (-3);
    
    root->set_relative_z(false);
    center->set_relative_z(false);
    orbit->set_relative_z(false);

    root->set_image_texture( ImageResource::get_resource("res/gold.png") );
    center->set_script_resource( LuaScriptResource::get_resource("res/scripts/test.lua") );
    orbit->set_image_texture( ImageResource::get_resource("res/gold.png") );


    root->add_child_node(center);
    center->add_child_node(orbit);

    center->set_position( Vector2(55,55) );
    center->set_rotation_degrees( 45 );
    orbit->set_position( Vector2(55,55) );

    

    while( !window.should_close() ){
        window.update();
    }


    std::cout << "Bye bye!!!" << std::endl;
}