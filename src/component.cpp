#include <iostream>
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
    std::cerr << "Component::attach_node shouldn't be called. Proper specialize it" << std::endl;
}
void        Component::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( Component , get_node );
}