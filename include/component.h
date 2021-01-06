#ifndef COMPONENT_H
#define COMPONENT_H

class SceneNode;

#include "saucer_object.h"

class Component : public SaucerObject {
    
    #define REGISTER_AS_INHERITED_COMPONENT(C,ParentComponent)\
    REGISTER_SAUCER_OBJECT(C,ParentComponent);                              \
    friend class Scene;                                                     \
    private:                                                                \
        static std::unordered_map< SaucerId , C* > component_from_node;     \
    public:                                                                 \
        static C*  recover_from_node( const SceneNode* node ){              \
            if(!node) return nullptr;                                       \
            auto find = component_from_node.find( node->get_saucer_id() );  \
            if( find == component_from_node.end() ) return nullptr;         \
            else return find->second;                                       \
        }                                                                   \
    protected:                                                              \
        void        attach_node( SceneNode* node ){                         \
            attached_node = node;                                           \
            component_from_node[node->get_saucer_id()] = this;              \
            ParentComponent::attach_node(node);                             \
        }
    
    #define REGISTER_AS_COMPONENT(C) REGISTER_AS_INHERITED_COMPONENT(C,Component);
    
    
    
    REGISTER_SAUCER_OBJECT(Component,SaucerObject);

    friend class SceneNode;
    protected:
        virtual void        attach_node( SceneNode* node );    

    protected:
        SceneNode*  attached_node = nullptr;
        Component();
        ~Component();

    public:
        SceneNode*          get_node() const;
        static  void        bind_methods();
    
};

#include "scene_node.h"


// Registering "per-component" get_component/set_component functions, since we can't use get_component<Sprite>() in Lua!
#define REGISTER_COMPONENT_HELPERS(C,lowercase_name)                                                                    \
LuaEngine::register_function( "SceneNode" , std::string("get_")+std::string(lowercase_name) , [](lua_State* ls){        \
    SceneNode* node = LuaEngine::pop<SceneNode*>(ls);                                                                   \
    LuaEngine::push( ls , node->get_component<C>() );                                                                   \
    return 1;                                                                                                           \
});                                                                                                                     \
LuaEngine::register_function( "SceneNode" , std::string("create_")+std::string(lowercase_name) , [](lua_State* ls){     \
    SceneNode* node = LuaEngine::pop<SceneNode*>(ls);                                                                   \
    LuaEngine::push( ls , node->create_component<C>() );                                                                \
    return 1;                                                                                                           \
});                                                                                                                     \
LuaEngine::register_function( "SceneNode" , std::string("destroy_")+std::string(lowercase_name) , [](lua_State* ls){    \
    SceneNode* node = LuaEngine::pop<SceneNode*>(ls);                                                                   \
    node->destroy_component<C>();                                                                                       \
    return 0;                                                                                                           \
});   

#endif