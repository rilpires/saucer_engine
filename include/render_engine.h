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

// This is the data to be passed to the render engine.
// Proper transforms from SceneNode will be applied further, outside of this scope.
// Since we are talking only about sprites, it is basically:
// 1 - TextureID
// 2 - UV coordinates ( two pairs of vector2 while we don't have a Rect2 type )
struct RenderData {
    GLuint              texture_id;
    Vector2             uv_top_left;
    Vector2             uv_bottom_right;
    Vector2             size_in_pixels;
    ShaderResource*     shader_program;
    bool                use_tree_transform;
    bool                view_transform;
    bool                tex_is_alpha_mask = false;
    Transform           model_transform;  // This will be filled at "tree-discovering" time, since it got to be accumulated from parents
    Color               final_modulate; // This will be filled at "tree-discovering" time, since it got to be accumulated from parents
};
class RenderEngine {
    
    private:
        GLFWwindow*         glfw_window;
        std::string         window_title;
        Vector2             window_size;
        Transform           camera_transform;
        
        ShaderResource*     basic_shader_resource;
        ShaderResource*     current_shader_resource;

        GLuint              last_used_texture;

        bool                ignore_camera_atrib_location;
        GLuint              pixel_size_attrib_location;
        GLuint              camera_transf_attrib_location;
        GLuint              model_transf_attrib_location; 
        GLuint              modulate_attrib_location;
        GLuint              uv_div_attrib_location;
        GLuint              tex_alpha_mask_attrib_location;
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
        Transform           get_camera_transform() const ;
        void                set_camera_transform(Transform t);
        bool                should_close() const;
        std::string         get_window_title() const;
        void                set_window_title( std::string new_title );
        GLuint              get_last_used_texture() const;
        void                update( const std::vector<RenderData>& draws );


};

#endif