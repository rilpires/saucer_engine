#ifndef COMPONENT_H
#define COMPONENT_H

#include "saucer_object.h"

class SceneNode;


class Component : public SaucerObject {
    
    #define REGISTER_AS_COMPONENT(C) \
    REGISTER_SAUCER_OBJECT(C);                                                                              \
    private:                                                                                                        \
        static std::unordered_map< SaucerId , C* > component_from_node;                                             \
    public:                                                                                                         \
        static C*  recover_from_node( const SceneNode* node ){                                                      \
            if(!node) return nullptr;                                                                               \
            auto find = component_from_node.find( node->get_saucer_id() );                                          \
            if( find == component_from_node.end() ) return nullptr;                                                 \
            else return find->second;                                                                               \
        }                                                                                                           \
        void        attach_node( SceneNode* node ){                                                                 \
            if( attached_node ){                                                                                    \
                std::cerr << "Can't attach a node on a component that already has a node attached." << std::endl;   \
            }                                                                                                       \
            else if (!node){                                                                                        \
                std::cerr << "Cant't attach a node on null" << std::endl;                                           \
            }                                                                                                       \
            else if (component_from_node.find(node->get_saucer_id())!=component_from_node.end()){                   \
                std::cerr << "This node already has a \"" << #C << "\" component attached to it." << std::endl;     \
            }                                                                                                       \
            else {                                                                                                  \
                attached_node = node;                                                                               \
                component_from_node[node->get_saucer_id()] = this;                                                  \
            }                                                                                                       \
        }       
    

    protected:
        SceneNode*  attached_node = nullptr;


    public:
        Component();
        ~Component();

        SceneNode*          get_node() const;
        virtual void        attach_node( SceneNode* node );
    
};


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