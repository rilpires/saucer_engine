#include "render_engine.h"
#include "debug.h"
#include "resources/shader.h"
#include "render_object.h"
#include "collision.h"

#include <algorithm>

#define INITIAL_WINDOW_SIZE Vector2(640,480)
#define INITIAL_WINDOW_TITLE "SaucerEngine"


RenderEngine::RenderEngine(){
    
    if( !glfwInit() ) saucer_err( "Failed to glfwInit()" )
    
    glfw_window = glfwCreateWindow( INITIAL_WINDOW_SIZE.x , INITIAL_WINDOW_SIZE.y , INITIAL_WINDOW_TITLE , NULL , NULL );
    
    // Defining context variables & other stuffs
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR , 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR , 1 );
    // glfwWindowHint( GLFW_OPENGL_COMPAT_PROFILE , 0 );

    // Setting context
    glfwMakeContextCurrent(glfw_window);
    
    // Initializing GLEW
    glewExperimental = true;
    GLenum ret_glewInit = glewInit();
    if( ret_glewInit != GLEW_OK ) 
        saucer_err( "Failed to glewInit() :" , glewGetErrorString(ret_glewInit) )
    
    saucer_print( "Renderer: " , glGetString( GL_RENDERER )  );
    saucer_print( "Version: " , glGetString( GL_VERSION )  );

    last_used_texture = 0;

    GLint d = 0;
    GL_CALL( glGetIntegerv(GL_CONTEXT_PROFILE_MASK,&d))
    GL_CALL( saucer_log( "GL_CONTEXT_COMPATIBILITY_PROFILE_BIT = " , (d & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)))
    GL_CALL( saucer_log( "GL_CONTEXT_CORE_PROFILE_BIT = " , (d & GL_CONTEXT_CORE_PROFILE_BIT) ))
    d=0;
    GL_CALL( glGetIntegerv(GL_CONTEXT_FLAGS, &d))
    GL_CALL( saucer_log( "GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT" , (d & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)))
    GL_CALL( saucer_log( "GL_CONTEXT_FLAG_DEBUG_BIT" , (d & GL_CONTEXT_FLAG_DEBUG_BIT)))
    GL_CALL( saucer_log( "GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT" , (d & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT)))
    GL_CALL( saucer_log( "GL_CONTEXT_FLAG_NO_ERROR_BIT" , (d & GL_CONTEXT_FLAG_NO_ERROR_BIT)))

    GL_CALL( saucer_log( "GL_SHADING_LANGUAGE_VERSION = " , glGetString(GL_SHADING_LANGUAGE_VERSION)))
    GL_CALL( glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS,&d); )
    GL_CALL( for( int i = 0 ; i < d ; i++ ) saucer_log(glGetStringi(GL_SHADING_LANGUAGE_VERSION,i)))

    basic_shader_resource = (ShaderResource*) ResourceManager::get_resource("res/shaders/basic.glsl");
    set_current_shader( basic_shader_resource );
    camera_transform = Transform();


    Vector2 window_size = get_window_size();
    // VBO (Vertex Buffer Object) for 2D objects, constant
    // These will be properly transformed by uniforms:
    // 1 - scaled proportionally to texture_size from window_size (gl_Position.xy *= textureSize(tex,0)/viewport_size;)
    // 2 - model transform (gl_Position *= model_transf)
    // 3 - camera transform (gl_Position *= camera_transf)
    // 4 - finally, scaling from screen_space coordinates to window_space ( gl_Position.xy /= ( viewport_size/2 ) )
    float vertex_data[] = {
    /*  X                       Y                       Z       U       V    */
        -0.5f*window_size.x ,   -0.5f*window_size.y ,   0.0 ,   0.0 ,   1.0 ,
        +0.5f*window_size.x ,   -0.5f*window_size.y ,   0.0 ,   1.0 ,   1.0 ,
        +0.5f*window_size.x ,   +0.5f*window_size.y ,   0.0 ,   1.0 ,   0.0 ,
        -0.5f*window_size.x ,   +0.5f*window_size.y ,   0.0 ,   0.0 ,   0.0
    };
    unsigned int vbo;
    GL_CALL( glGenBuffers(1, &vbo) );
    GL_CALL( glBindBuffer(GL_ARRAY_BUFFER,vbo) );
    GL_CALL( glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) , vertex_data , GL_STATIC_COPY ) );

    // VAO (Vertex Array Object) for generic sprites
    unsigned int vao;
    GL_CALL( glCreateVertexArrays(1,&vao) );
    GL_CALL( glBindVertexArray(vao) );
    GL_CALL( glVertexAttribPointer( 0 , 3 , GL_FLOAT , false , 5*sizeof(GL_FLOAT) , 0 ));
    GL_CALL( glVertexAttribPointer( 1 , 2 , GL_FLOAT , false , 5*sizeof(GL_FLOAT) , (void*)(3*sizeof(GL_FLOAT)) ));
    GL_CALL( glEnableVertexAttribArray(  0 ) );
    GL_CALL( glEnableVertexAttribArray( 1 ) );

    unsigned int vertex_index[] = { 
        1 , 2 , 0 ,
        3 };
    
    unsigned int veb;
    GL_CALL( glGenBuffers(1,&veb) );
    GL_CALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,veb) );
    GL_CALL( glBufferData(GL_ELEMENT_ARRAY_BUFFER , sizeof(vertex_index) , vertex_index , GL_STATIC_DRAW) );


    GL_CALL( glEnable(GL_DEPTH_TEST) );
    GL_CALL( glEnable(GL_BLEND) );
    GL_CALL( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );  
    GL_CALL( glDepthFunc(GL_LEQUAL) );

}
RenderEngine::~RenderEngine(){
    glfwDestroyWindow( glfw_window );
    glfwTerminate();
}
GLFWwindow*         RenderEngine::get_glfw_window() const {
    return glfw_window;
};
void                RenderEngine::set_current_shader( ShaderResource* new_shader_res ){
    if( new_shader_res != current_shader_resource ){
        current_shader_resource = new_shader_res;
        if( current_shader_resource ){
            GL_CALL( glUseProgram(current_shader_resource->shader_program) );

            GL_CALL( viewport_size_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"viewport_size")  );
            GL_CALL( camera_transf_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"camera_transf")  );
            GL_CALL( model_transf_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"model_transf")  );
            GL_CALL( modulate_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"in_modulate")  );
            GL_CALL( uv_div_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"uv_div")  );
            
            GL_CALL( glUniform2f( viewport_size_attrib_location , get_window_size().x , get_window_size().y ) );
            GL_CALL( glUniformMatrix4fv( camera_transf_attrib_location , 1 , GL_FALSE , camera_transform.m ) );
        }
    }
}
void                RenderEngine::set_window_size( Vector2 new_size ){
    glfwSetWindowSize( glfw_window , new_size.x , new_size.y );
    GL_CALL( glUniform2f( viewport_size_attrib_location , get_window_size().x , get_window_size().y ) );
};
Vector2             RenderEngine::get_window_size() const {
    int w,h;
    glfwGetWindowSize( glfw_window , &w , &h );
    return Vector2( w,h );
};
void                RenderEngine::set_window_pos( Vector2 new_pos ){
    glfwSetWindowPos( glfw_window , new_pos.x , new_pos.y );
}
Vector2             RenderEngine::get_window_pos() const {
    int x,y;
    glfwGetWindowPos( glfw_window , &x , &y );
    return Vector2(x,y);
}
void                RenderEngine::set_fullscreen( bool fs ){
    if( !is_fullscreen() && fs ){
        int w,h;
        glfwGetWindowSize( glfw_window , &w , &h );
        glfwSetWindowMonitor( glfw_window , glfwGetPrimaryMonitor() , 0 , 0 , w , h , 60 );
    } else if( is_fullscreen() && !fs ) {
        int w,h;
        glfwGetWindowSize( glfw_window , &w , &h );
        glfwSetWindowMonitor( glfw_window , NULL , 0 , 0 , w , h , 60 );
    }
}
bool                RenderEngine::is_fullscreen() const {
    return glfwGetWindowMonitor(glfw_window) != NULL;
};
Transform           RenderEngine::get_camera_transform() const {
    return camera_transform;
}
void                RenderEngine::set_camera_transform(Transform t){
    camera_transform = t;
    GL_CALL( glUniformMatrix4fv( camera_transf_attrib_location , 1 , GL_FALSE , camera_transform.m ) );
}
bool                RenderEngine::should_close() const {
    return glfwWindowShouldClose(glfw_window);
}
std::string         RenderEngine::get_window_title() const {
    return window_title;
}
void                RenderEngine::set_window_title( std::string new_title ){
    window_title = new_title;
    glfwSetWindowTitle( glfw_window , new_title.c_str() );
}
GLuint              RenderEngine::get_last_used_texture() const{
    return last_used_texture;
}
void                RenderEngine::update( const std::vector<RenderData>& draws ){
    
    GL_CALL( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
    GL_CALL( glClearColor( 0,0,0,1 ) );
    for( auto render_data = draws.begin() ; render_data != draws.end() ; render_data ++ ){
            
        ShaderResource* shader_program = render_data->shader_program;
        if(!shader_program) shader_program = basic_shader_resource;
        set_current_shader( shader_program );
        
        if( render_data->texture_id ) {
            
            const float uv_div[4] = { render_data->uv_top_left.x      ,
                                      render_data->uv_top_left.y      , 
                                      render_data->uv_bottom_right.x  ,
                                      render_data->uv_bottom_right.y  };
            
            if( last_used_texture != render_data->texture_id ){
                GL_CALL( glBindTexture( GL_TEXTURE_2D , render_data->texture_id ) );
                last_used_texture = render_data->texture_id;
            }
            GL_CALL( glUniformMatrix4fv( model_transf_attrib_location , 1 , GL_FALSE , render_data->model_transform.m ) );
            GL_CALL( glUniform4fv( uv_div_attrib_location , 1 , uv_div ) );
            GL_CALL( glUniform4f( modulate_attrib_location , ((float)render_data->final_modulate.r)/255.0f , 
                                                             ((float)render_data->final_modulate.g)/255.0f , 
                                                             ((float)render_data->final_modulate.b)/255.0f , 
                                                             ((float)render_data->final_modulate.a)/255.0f ));
            GL_CALL( glDrawElements(GL_TRIANGLE_STRIP,4,GL_UNSIGNED_INT,nullptr ) );

        }
    }
    
    glfwSwapBuffers( glfw_window );

};


//