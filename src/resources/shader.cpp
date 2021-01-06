#include "resources/shader.h"
#include "debug.h"
#include <iostream>

ShaderResource::ShaderResource( std::string filename ){
    shader_program = glCreateProgram();
    std::string shader_file = read_file_as_str( filename );
    std::string vertex_shader;
    std::string frag_shader;

    size_t vertex_header;
    size_t vertex_src_begin;
    size_t frag_header;
    size_t frag_src_begin;

    vertex_header = shader_file.find("#vertex");
    frag_header = shader_file.find("#fragment");
    if( vertex_header == std::string::npos || frag_header == std::string::npos )
        std::cerr << "Error while parsing shader file \"" << filename << "\", unexpected behavior ahead" <<std::endl;
    vertex_src_begin = shader_file.find('\n',vertex_header)+1;
    frag_src_begin = shader_file.find('\n',frag_header)+1;
    vertex_shader = shader_file.substr( vertex_src_begin , frag_header-vertex_src_begin );
    frag_shader = shader_file.substr( frag_src_begin );
    attach_shader( GL_VERTEX_SHADER , vertex_shader );
    attach_shader( GL_FRAGMENT_SHADER , frag_shader );
    GL_CALL( glLinkProgram(shader_program) );
    // GL_CALL( glBindFragDataLocation( shader_program, 0, "outColor") ); 
}
ShaderResource::~ShaderResource(){
    glDeleteProgram( shader_program );
}

void    ShaderResource::attach_shader( GLenum shader_type , std::string shader_src ){

    int src_size = shader_src.size();
    const char* src_c_str = shader_src.c_str();

    GL_CALL( unsigned int shader = glCreateShader( shader_type ) );
    GL_CALL( glShaderSource( shader , 1 , &src_c_str , &src_size ) );
    GL_CALL( glCompileShader( shader ) );
    
    int gl_compile_status;
    glGetShaderiv( shader , GL_COMPILE_STATUS , &gl_compile_status );
    if( gl_compile_status == GL_FALSE ){
        int info_log_size;
        glGetShaderiv( shader ,GL_INFO_LOG_LENGTH,&info_log_size);
        char* msg = new char[info_log_size];
        glGetShaderInfoLog( shader ,info_log_size,&info_log_size, msg);
        std::cerr << "[OpenGL Error] Couldn't compile " << ((shader_type==GL_VERTEX_SHADER)?("vertex"):("fragment")) << " shader " << std::endl
        << msg << std::endl;
        delete[] msg;
    }
    GL_CALL( glAttachShader( shader_program , shader ); )
    GL_CALL( glDeleteShader( shader ); )
}
void    ShaderResource::bind_methods(){

}
