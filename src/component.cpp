#include "debug.h"
#include "component.h"
#include "lua_engine.h"

Component::Component(){

}
Component::~Component(){

}

SceneNode*  Component::get_node() const{
    return attached_node;
}
void        Component::attach_node( SceneNode* node ){
    // saucer_err("Do we care?"); // No we do not care
}
void        Component::entered_tree(){

}
void        Component::exiting_tree(){

}
void        Component::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( Component , get_node );
}