#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#define GLFW_INCLUDE_NONE


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "transform.h"

class RenderObject;
class ShaderResource;
class RenderEngine {
    
    private:
        GLFWwindow*         glfw_window;
        std::string         window_title;
        Transform           camera_transform;
        
        ShaderResource*     basic_shader_resource;
        ShaderResource*     current_shader_resource;

        GLuint              viewport_size_attrib_location;
        GLuint              camera_transf_attrib_location;
        GLuint              model_transf_attrib_location; 
        GLuint              modulate_attrib_location;
        GLuint              uv_div_attrib_location;
        
        
    public:
        RenderEngine();
        ~RenderEngine();

        GLFWwindow*         get_glfw_window() const;
        void                set_current_shader( ShaderResource* new_shader_res );
        void                set_window_size( Vector2 new_size );
        Vector2             get_window_size() const;
        void                set_window_pos( Vector2 new_pos );
        Vector2             get_window_pos() const ;
        void                set_fullscreen( bool fs );
        bool                is_fullscreen() const ;
        Transform           get_camera_transform() const ;
        void                set_camera_transform(Transform t);
        bool                should_close() const;
        std::string         get_window_title() const;
        void                set_window_title( std::string new_title );
        void                update( std::vector<RenderObject*>& draws );


};

#endif