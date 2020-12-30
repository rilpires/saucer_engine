#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include <iostream>
#include <vector>
#include <unordered_map>

#include "input.h"
#include "vector.h"
#include "resources.h"
#include "resources/image.h"
#include "saucer_object.h"

class Scene;
class LuaScriptResource;

class SceneNode : public SaucerObject {
    friend class Scene;

    protected:
        Vector2                     position;
        float                       rotation_degrees; // Counter clock wise
        short                       z;
        bool                        relative_z;
        SceneNode*                  parent_node;
        std::vector<SceneNode*>     children_nodes;
        Scene*                      scene;
        ImageResource*              image_texture;
        LuaScriptResource*          lua_script;

    public:
        SceneNode();
        ~SceneNode();

        static SceneNode*       lua_new();
        void                    set_position( const Vector2 new_pos );
        Vector2                 get_position( ) const;
        Vector2                 get_global_position() const;
        void                    set_rotation_degrees( float new_rotation_degrees_cw );
        float                   get_rotation_degrees( );
        float                   get_global_rotation_degrees() const;
        Transform               get_global_transform() const ;
        void                    set_z( short new_z );
        short                   get_z( ) const;
        short                   get_global_z() const ;
        void                    set_relative_z(bool new_val);
        bool                    is_z_relative() const ;
        void                    set_image_texture( ImageResource* img );
        ImageResource*          get_image_texture() const ;
        void                    set_script_resource( LuaScriptResource* ls );
        LuaScriptResource*      get_script_resource() const ;
        void                    get_out();
        void                    add_child( SceneNode* p_child_node );
        SceneNode*              get_parent( ) const ;
        Scene*                  get_scene() const;

        std::vector<SceneNode*> const&  get_children() const;


        inline virtual void     entered_scene(){};
        inline virtual void     process_input_event( Input::InputEvent* input_event ){};

    public:
        static void         bind_methods();

};


#endif