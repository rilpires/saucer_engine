#ifndef COMPONENT_H
#define COMPONENT_H

class SceneNode;

#include "saucer_object.h"

class Component : public SaucerObject {
    
    #define REGISTER_AS_INHERITED_COMPONENT(C,ParentComponent)\
    REGISTER_SAUCER_OBJECT(C,ParentComponent);                                                              \
    friend class Scene;                                                                                     \
    private:                                                                                                \
        static std::unordered_multimap< SaucerId , C* >& component_from_node(){                             \
            static std::unordered_multimap< SaucerId , C* > r;                                              \
            return r;                                                                                       \
        };                                                                                                  \
        using iterator = std::unordered_multimap< SaucerId , C* >::iterator;                                \
    public:                                                                                                 \
        static C*  recover_from_node( const SceneNode* node ){                                              \
            if(!node) return nullptr;                                                                       \
            auto it_range = component_from_node().equal_range( node->get_saucer_id() );                     \
            if( it_range.first == component_from_node().end() ) return nullptr;                             \
            else {                                                                                          \
                C* ret = (it_range.first)->second;                                                          \
                return ret;                                                                                 \
             }                                                                                              \
        }                                                                                                   \
        static std::pair<iterator,iterator>  recover_range_from_node( const SceneNode* node ){              \
            return component_from_node().equal_range( node->get_saucer_id() );                              \
        }                                                                                                   \
        virtual void  erase_from_component_map(){                                                           \
            auto it_range = component_from_node().equal_range(attached_node->get_saucer_id());              \
            for( auto it = it_range.first ; it != it_range.second ; it++ )                                  \
            if( it->second == this ){                                                                       \
                component_from_node().erase(it);break;                                                      \
            }                                                                                               \
            parent_type::erase_from_component_map();                                                        \
        }                                                                                                   \
        virtual std::string get_component_name() const{ return std::string(#C); }                           \
                                                                                                            \
    protected:                                                                                              \
        void        attach_node( SceneNode* node ){                                                         \
            attached_node = node;                                                                           \
            component_from_node().insert(std::make_pair<SaucerId,C*>(node->get_saucer_id(),this));          \
            ParentComponent::attach_node(node);                                                             \
        }                                                                                                   \
        
    
    #define REGISTER_AS_COMPONENT(C) REGISTER_AS_INHERITED_COMPONENT(C,Component);
    
    
    
    REGISTER_SAUCER_OBJECT(Component,SaucerObject);

    friend class SceneNode;
    protected:
        virtual void        attach_node( SceneNode* node );
        virtual void        erase_from_component_map();
    protected:
        SceneNode*          attached_node = nullptr;
        Component();
        ~Component();

    public:
        SceneNode*              get_node() const;
        virtual void            entered_tree();
        virtual void            exiting_tree();
        static  void            bind_methods();
        virtual void            push_editor_items();
        virtual std::string     get_component_name() const;
    
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
    node->create_component<C>();                                                                                        \
    LuaEngine::push( ls , node->get_component<C>() );                                                                   \
    return 1;                                                                                                           \
});                                                                                                                     \
LuaEngine::register_function( "SceneNode" , std::string("destroy_")+std::string(lowercase_name) , [](lua_State* ls){    \
    SceneNode* node = LuaEngine::pop<SceneNode*>(ls);                                                                   \
    node->destroy_component<C>();                                                                                       \
    return 0;                                                                                                           \
});                                                                                                                     \
__component_constructors[C::class_name] = (void(SceneNode::*)())&SceneNode::create_component<C>;  


#endif