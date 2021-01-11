#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include "component.h"
#include "engine.h"
#include "render_engine.h"


class RenderObject : public Component {
    REGISTER_AS_COMPONENT(RenderObject)

    friend class RenderEngine;

    private:
        bool                use_parent_shader;
        ShaderResource*     current_shader;
    protected:
        VertexData*         vertex_data;
        unsigned short      vertex_data_count;
        bool                dirty_vertex_data; // useful for some big VBOs
        RenderObject();
        
    public:
        virtual std::vector<RenderData>  generate_render_data() ;

        bool                get_use_parent_shader() const ;
        void                set_use_parent_shader( bool new_val );
        ShaderResource*     get_current_shader() const ;
        void                set_current_shader( ShaderResource* p );

        static void         bind_methods();
};



#endif