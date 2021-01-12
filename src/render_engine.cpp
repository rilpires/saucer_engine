#include "render_engine.h"
#include "debug.h"
#include "resources/shader.h"
#include "render_object.h"
#include "collision.h"

#include <algorithm>

#define INITIAL_WINDOW_SIZE Vector2(640,480)
#define INITIAL_WINDOW_TITLE "SaucerEngine"

void RenderData::fill_vertices_modulate(){
    for( unsigned short i = 0 ; i < vertex_data_count ; i++ ) vertex_data[i].modulate = final_modulate;
}

RenderEngine::RenderEngine(){
    
    if( !glfwInit() ) saucer_err( "Failed to glfwInit()" )
    
    window_size = INITIAL_WINDOW_SIZE;
    glfw_window = glfwCreateWindow( window_size.x , window_size.y , INITIAL_WINDOW_TITLE , NULL , NULL );
    
    // Defining context variables & other stuffs
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR , 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR , 0 );
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
    view_transform = Transform();
    basic_shader_resource = (ShaderResource*) ResourceManager::get_resource("res/shaders/basic.glsl");

    set_current_shader( basic_shader_resource );
    set_window_size(INITIAL_WINDOW_SIZE);


    GL_CALL( glGenBuffers(1, &vbo_index) );
    GL_CALL( glBindBuffer(GL_ARRAY_BUFFER,vbo_index) );
    GL_CALL( glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData)*MAX_VERTEX_COUNT , 0 , GL_STREAM_DRAW ) );
    GL_CALL( m_VBO = (VertexData*)glMapBuffer(GL_ARRAY_BUFFER , GL_WRITE_ONLY ) );
    
    // VAO (Vertex Array Object) for generic sprites
    unsigned int vao;
    GL_CALL( glCreateVertexArrays(1,&vao) );
    GL_CALL( glBindVertexArray(vao) );
    GL_CALL( glVertexAttribPointer( 0 , 3 , GL_FLOAT ,          false , sizeof(VertexData) , 0 ));
    GL_CALL( glVertexAttribPointer( 1 , 2 , GL_FLOAT ,          false , sizeof(VertexData) , (void*)(sizeof(Vector3))));
    GL_CALL( glVertexAttribPointer( 2 , 4 , GL_UNSIGNED_BYTE ,  true , sizeof(VertexData) , (void*)(sizeof(Vector3)+sizeof(Vector2))));
    GL_CALL( glEnableVertexAttribArray( 0 ) );
    GL_CALL( glEnableVertexAttribArray( 1 ) );
    GL_CALL( glEnableVertexAttribArray( 2 ) );

    unsigned int veb;
    unsigned short veb_array[MAX_VERTEX_COUNT];
    for( size_t i = 0 , offset=0 ; i < MAX_VERTEX_COUNT ; i+=6 , offset+=4 ){
        veb_array[i+0] = offset+0;
        veb_array[i+1] = offset+1;
        veb_array[i+2] = offset+2;
        veb_array[i+3] = offset+1;
        veb_array[i+4] = offset+2;
        veb_array[i+5] = offset+3;
    }
    GL_CALL( glGenBuffers(1,&veb) );
    GL_CALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,veb) );
    GL_CALL( glBufferData(GL_ELEMENT_ARRAY_BUFFER , sizeof(unsigned short)*MAX_VERTEX_COUNT , veb_array , GL_STATIC_DRAW) );


    GL_CALL( glEnable(GL_DEPTH_TEST) );
    GL_CALL( glEnable(GL_BLEND) );
    GL_CALL( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );  
    GL_CALL( glDepthFunc(GL_LEQUAL) );

    //GLint d = 0;
    //GL_CALL( glGetIntegerv(GL_CONTEXT_PROFILE_MASK,&d))
    //GL_CALL( saucer_log( "GL_CONTEXT_COMPATIBILITY_PROFILE_BIT = " , (d & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)))
    //GL_CALL( saucer_log( "GL_CONTEXT_CORE_PROFILE_BIT = " , (d & GL_CONTEXT_CORE_PROFILE_BIT) ))
    //d=0;
    //GL_CALL( glGetIntegerv(GL_CONTEXT_FLAGS, &d))
    //GL_CALL( saucer_log( "GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT" , (d & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)))
    //GL_CALL( saucer_log( "GL_CONTEXT_FLAG_DEBUG_BIT" , (d & GL_CONTEXT_FLAG_DEBUG_BIT)))
    //GL_CALL( saucer_log( "GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT" , (d & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT)))
    //GL_CALL( saucer_log( "GL_CONTEXT_FLAG_NO_ERROR_BIT" , (d & GL_CONTEXT_FLAG_NO_ERROR_BIT)))
    //GL_CALL( saucer_log( "GL_SHADING_LANGUAGE_VERSION = " , glGetString(GL_SHADING_LANGUAGE_VERSION)))
    //GL_CALL( glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS,&d); )
    //GL_CALL( for( int i = 0 ; i < d ; i++ ) saucer_log(glGetStringi(GL_SHADING_LANGUAGE_VERSION,i)))
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

            GL_CALL( view_transf_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"view_transf")  );
            GL_CALL( model_transf_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"model_transf")  );
            GL_CALL( viewport_size_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"viewport_size")  );
            GL_CALL( ignore_camera_atrib_location = glGetUniformLocation(current_shader_resource->shader_program,"ignore_camera")  );
            GL_CALL( tex_is_alpha_mask_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"tex_is_alpha_mask")  );
            GL_CALL( modulate_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"uniform_modulate")  );
            GL_CALL( time_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"time")  );
            GL_CALL( glUniform2f( viewport_size_attrib_location , window_size.x , window_size.y ) );
            GL_CALL( glUniformMatrix4fv( view_transf_attrib_location , 1 , GL_FALSE , view_transform.m ) );
            GL_CALL( glUniform1f( time_attrib_location , Engine::get_uptime() ) );
        }
    }
}
void                RenderEngine::set_window_size( Vector2 new_size ){
    window_size = new_size;
    glfwSetWindowSize( glfw_window , window_size.x , window_size.y );
    GL_CALL( glUniform2f( viewport_size_attrib_location , window_size.x , window_size.y ) );
};
Vector2             RenderEngine::get_window_size() const {
    return window_size;
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
Transform           RenderEngine::get_view_transform() const {
    return view_transform;
}
void                RenderEngine::set_view_transform(Transform t){
    view_transform = t;
    GL_CALL( glUniformMatrix4fv( view_transf_attrib_location , 1 , GL_FALSE , view_transform.m ) );
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
ShaderResource*     RenderEngine::get_basic_shader() const{
    return basic_shader_resource;
}
void                RenderEngine::update( const std::vector<RenderData>& draws ){
    
    GL_CALL( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
    GL_CALL( glClearColor( 0,0,0,1 ) );
    GL_CALL( glUniform1f( time_attrib_location , Engine::get_uptime() ) );
    
    size_t first=0 , batch_size=MAX_VERTEX_COUNT+1;
    for( size_t i = 0 ; i < draws.size() ; i++ ){
        const RenderData& render_data = draws[i];
        
        for( size_t k = 0 ; k < render_data.vertex_data_count ; k++ )
            m_VBO[k] = render_data.vertex_data[k];
        
        if( current_shader_resource != render_data.shader_program )
            set_current_shader( render_data.shader_program );
        
        if( last_used_texture != render_data.texture_id ){
            GL_CALL( glBindTexture( GL_TEXTURE_2D , render_data.texture_id ) );
            last_used_texture = render_data.texture_id;
        }

        float modulate_as_float[] = {
            float(render_data.final_modulate.r)/255.0f,
            float(render_data.final_modulate.g)/255.0f,
            float(render_data.final_modulate.b)/255.0f,
            float(render_data.final_modulate.a)/255.0f,
        };
        GL_CALL( glUniform1i( ignore_camera_atrib_location , !render_data.use_view_transform  ) );
        GL_CALL( glUniform1i( tex_is_alpha_mask_attrib_location , render_data.tex_is_alpha_mask ) );
        GL_CALL( glUniform4fv( modulate_attrib_location , 1 , modulate_as_float ) );
        GL_CALL( glUniformMatrix4fv( model_transf_attrib_location , 1 , false , render_data.model_transform.m ) );
        GL_CALL( glUnmapBuffer(GL_ARRAY_BUFFER) );
        GL_CALL( glDrawElements(GL_TRIANGLES,(render_data.vertex_data_count/4)*6,GL_UNSIGNED_SHORT,nullptr ) );
        GL_CALL( m_VBO = (VertexData*)glMapBuffer(GL_ARRAY_BUFFER , GL_WRITE_ONLY ) );
    }

    glfwSwapBuffers( glfw_window );

};


//