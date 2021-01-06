#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include "component.h"
#include "engine.h"
#include "render_engine.h"


class RenderObject : public Component {
    REGISTER_AS_COMPONENT(RenderObject)

    friend class RenderEngine;
    protected:
        // This is the data to be passed to the render engine.
        // Proper transforms from SceneNode will be applied further, outside of this scope.
        // Since we are talking only about sprites, it is basically:
        // 1 - TextureID
        // 2 - UV coordinates ( two pairs of vector2 while we don't have a Rect2 type )
        struct RenderData {
            GLuint  texture_id;
            Vector2 uv_top_left;
            Vector2 uv_bottom_right;
        };

    private:
        Color           modulate;
        Color           self_modulate;
        bool            use_parent_shader;
        ShaderResource* current_shader;
        
    protected:
        RenderObject();
        
    public:
        virtual RenderData      get_render_data() const ;

        Color           get_modulate() const ;
        void            set_modulate( Color new_col );
        Color           get_self_modulate() const ;
        void            set_self_modulate( Color new_col );
        Color           get_global_modulate() const ;
        bool            get_use_parent_shader() const ;
        void            set_use_parent_shader( bool new_val );
        ShaderResource* get_current_shader() const ;
        void            set_current_shader( ShaderResource* p );

        static void     bind_methods();
};



#endif