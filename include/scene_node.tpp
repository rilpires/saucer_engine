#ifndef SCENE_NODE_TPP
#define SCENE_NODE_TPP


#include "component.h"


template< typename T> 
T*      SceneNode::get_component() const{
    return T::recover_from_node(this);
};
template< typename T> 
T*      SceneNode::create_component( ){
    if( get_component<T>() ){
        std::cout << "Warning: trying to create a component of a type that already exists" << std::endl;
        return nullptr;
    }
    else {
        T* new_comp = new T();
        attached_components.push_back(new_comp);
        ((Component*)new_comp)->attach_node(this);
        return new_comp;
    }
}
template< typename T> 
void    SceneNode::destroy_component( ){
    T* current_comp = get_component<T>();
    if( !current_comp ){
        std::cout << "Warning: trying to destroy an unexistent component " << std::endl;
    } else {
        auto it = attached_components.begin();
        while( *it != (Component*)current_comp ) it++;
        attached_components.erase(it);
        delete current_comp;
    }
}
        


#endif