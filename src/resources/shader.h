#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "resources.h"
#include "render_engine.h"

class ShaderResource : public Resource {
    REGISTER_SAUCER_OBJECT(ShaderResource,Resource)
    
    friend class RenderEngine;
    friend class RenderObject;
    private:
        GLuint  shader_program;

        void    attach_shader( GLenum shader_type , std::string shader_src );

    public:
        ShaderResource( const std::vector<uint8_t>& mem_data );
        ~ShaderResource();

        

        static void bind_methods();

};

#endif