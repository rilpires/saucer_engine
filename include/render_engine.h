#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#define GLFW_INCLUDE_NONE


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "transform.h"
#include "color.h"

class RenderObject;
class ShaderResource;

#define MAX_VERTEX_COUNT 50000 // We only aim for 2D so

struct VertexData{
    Vector3 pos;
    Vector2 uv;
    Color modulate;
};
struct RenderData {
    GLuint              texture_id = 0;
    VertexData*         vertex_data = nullptr;
    short               vertex_data_count = 0;
    ShaderResource*     shader_program = nullptr;
    bool                use_tree_transform = true;
    bool                use_view_transform = true;
    bool                tex_is_alpha_mask = false; // Useful for fonts...
    Transform           model_transform;  // If "use_tree_transform" is true, this will be filled at "tree-discovering" time, since it got to be accumulated from parents
    Color               final_modulate; // This will be filled at "tree-discovering" time, since it got to be accumulated from parents
    void                fill_vertices_modulate();     
};
class RenderEngine {
    
    private:

        GLFWwindow*         glfw_window;
        std::string         window_title;
        Vector2             window_size;
        
        Transform           view_transform;
        
        ShaderResource*     basic_shader_resource;
        ShaderResource*     current_shader_resource;

        GLuint              last_used_texture;
        
        GLuint              vbo_index;
        VertexData*         m_VBO;

        GLuint              ignore_camera_atrib_location;
        GLuint              view_transf_attrib_location;
        GLuint              model_transf_attrib_location; 
        GLuint              viewport_size_attrib_location;
        GLuint              tex_is_alpha_mask_attrib_location;
        GLuint              time_attrib_location;
        
        
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
        Transform           get_view_transform() const ;
        void                set_view_transform(Transform t);
        bool                should_close() const;
        std::string         get_window_title() const;
        void                set_window_title( std::string new_title );
        GLuint              get_last_used_texture() const;
        ShaderResource*     get_basic_shader() const;
        void                update( const std::vector<RenderData>& draws );


};

#endif