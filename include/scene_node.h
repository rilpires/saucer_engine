#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include "debug.h"
#include <vector>
#include <list>
#include <unordered_map>
#include "input.h"
#include "vector.h"
#include "resources.h"
#include "resources/image.h"
#include "resources/node_template.h"
#include "saucer_object.h"

class Component;
class Scene;
class LuaScriptResource;

class SceneNode : public SaucerObject {

    friend class Scene;
    REGISTER_SAUCER_OBJECT(SceneNode,SaucerObject);

    private:
        std::string                 name;
        Vector2                     position;
        Vector2                     scale;
        float                       rotation_degrees; // Counter clock wise
        short                       z;
        bool                        relative_z;
        Color                       modulate;
        Color                       self_modulate;
        bool                        inherits_transform;
        SceneNode*                  parent_node;
        std::vector<SceneNode*>     children_nodes;
        Scene*                      scene;
        LuaScriptResource*          lua_script;
        std::list<Component*>       attached_components;
        bool                        visible;
    
        void                    set_scene(Scene* new_scene);
    public:
        SceneNode();
        ~SceneNode();

        static SceneNode*       lua_new();
        std::string             get_name() const;
        void                    set_name( std::string new_val );
        void                    set_position( const Vector2 new_pos );
        Vector2                 get_position( ) const;
        void                    set_scale( const Vector2 new_scale );
        Vector2                 get_scale( ) const;
        void                    set_global_position( const Vector2 new_pos );
        Vector2                 get_global_position() const;
        void                    set_rotation_degrees( float new_rotation_degrees_cw );
        float                   get_rotation_degrees( ) const ;
        float                   get_global_rotation_degrees() const;
        Transform               get_transform() const ;
        Transform               get_global_transform() const ;
        void                    set_z( short new_z );
        short                   get_z( ) const;
        short                   get_global_z() const ;
        Color                   get_modulate() const ;
        void                    set_modulate( Color new_col );
        Color                   get_self_modulate() const ;
        void                    set_self_modulate( Color new_col );
        Color                   get_global_modulate() const ;
        void                    set_relative_z(bool new_val);
        bool                    get_relative_z() const ;
        bool                    get_visible() const;
        void                    set_visible( bool new_val );
        void                    set_script( LuaScriptResource* ls );
        LuaScriptResource*      get_script() const ;
        void                    get_out();
        bool                    is_parent_of( SceneNode* ) const;
        void                    add_child( SceneNode* p_child_node );
        SceneNode*              get_node( std::string child_name ) const;
        SceneNode*              get_parent( ) const ;
        Scene*                  get_scene() const;
        void                    queue_free();
        NodeTemplateResource*   pack_as_resource() const;
        SceneNode*              duplicate() const;


        // These are public but should not be binded to lua:

        std::vector<SceneNode*> const&  get_children() const;

        bool                    get_inherits_transform() const;
        void                    set_inherits_transform(bool new_val);

        template< typename T> 
        T*                      get_component() const;
        template< typename T> 
        void                    create_component( );
        template< typename T> 
        void                    destroy_component( );
        void                    destroy_component( Component* c );


        std::vector<Component*> get_attached_components() const;

        static const std::unordered_map<std::string, void(SceneNode::*)() >& __get_component_constructors();
    private:
        static std::unordered_map<std::string, void(SceneNode::*)() > __component_constructors;

        void                entered_tree();
        void                exiting_tree();

    public:
        static void         bind_methods();
        YamlNode            to_yaml_node() const override ;
        void                from_yaml_node( YamlNode ) override ;

};

#include "scene_node.tpp"

#endif