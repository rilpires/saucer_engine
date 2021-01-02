#include <iostream>
#include "component.h"


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
