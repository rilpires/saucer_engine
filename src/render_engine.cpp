#include "render_engine.h"
#include "debug.h"
#include "resources/shader.h"
#include "render_object.h"
#include "collision.h"

#include <algorithm>

#define INITIAL_WINDOW_SIZE Vector2(640,480)
#define INITIAL_WINDOW_TITLE "SaucerEngine"


RenderEngine::RenderEngine(){
    
    if( !glfwInit() ) std::cerr << "Failed to glfwInit()" << std::endl;
    
    glfw_window = glfwCreateWindow( INITIAL_WINDOW_SIZE.x , INITIAL_WINDOW_SIZE.y , INITIAL_WINDOW_TITLE , NULL , NULL );
    
    // Defining context variables & other stuffs
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR , 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR , 0 );
    // glfwWindowHint( GLFW_OPENGL_COMPAT_PROFILE , 0 );

    // Setting context
    glfwMakeContextCurrent(glfw_window);
    
    // Initializing GLEW
    glewExperimental = true;
    GLenum ret_glewInit = glewInit();
    if( ret_glewInit != GLEW_OK ) std::cerr << "Failed to glewInit() :" << glewGetErrorString(ret_glewInit) << std::endl;
    
    // Logging stuffs
    const GLubyte* renderer = glGetString( GL_RENDERER );
    const GLubyte* version = glGetString( GL_VERSION );
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "Version: " << version << std::endl;

    basic_shader_resource = (ShaderResource*) ResourceManager::get_resource("res/shaders/basic.glsl");
    current_shader_resource = nullptr;
    camera_transform = Transform();

    // VAO (Vertex Array Object) for generic sprites
    unsigned int vao;
    GL_CALL( glCreateVertexArrays(1,&vao) );
    GL_CALL( glBindVertexArray(vao) );
    
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
    GL_CALL( glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) , vertex_data , GL_STATIC_DRAW ) );
    GL_CALL( glVertexAttribPointer( 0 , 3 , GL_FLOAT , false , 5*sizeof(GL_FLOAT) , 0 ));
    GL_CALL( glVertexAttribPointer( 1 , 2 , GL_FLOAT , false , 5*sizeof(GL_FLOAT) , (void*)(3*sizeof(GL_FLOAT)) ));
    GL_CALL( glEnableVertexAttribArray(  0 ) );
    GL_CALL( glEnableVertexAttribArray( 1 ) );

    // VEB (Vertex Element Buffer )
    // Just the two triangles
    unsigned int vertex_index[] = {
        0 , 2 , 1 ,
        0 , 3 , 2
    };

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

            GL_CALL( unsigned int viewport_size_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"viewport_size")  );
            GL_CALL( unsigned int camera_transf_attrib_location = glGetUniformLocation(current_shader_resource->shader_program,"camera_transf")  );
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
}
bool                RenderEngine::should_close() const {
    return glfwWindowShouldClose(glfw_window);
}
std::string         RenderEngine::get_window_title() const {
    return window_title;
}
void    RenderEngine::set_window_title( std::string new_title ){
    window_title = new_title;
    glfwSetWindowTitle( glfw_window , new_title.c_str() );
}
void    RenderEngine::update( std::vector<RenderObject*>& draws ){
    
    GL_CALL( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
    GL_CALL( glClearColor( 0,0,0,1 ) );
    
    // Z-sorting. Must be stable_sort or else unexpected "z flips" can occur between same z-level sprites sometimes...
    std::stable_sort( draws.begin() , draws.end() , []( const RenderObject* n1 , const RenderObject* n2)->bool{
        return n2->get_node()->get_global_z() > n1->get_node()->get_global_z();
    });

    for( auto it = draws.begin() ; it != draws.end() ; it ++ ){
        
        SceneNode* scene_node = (*it)->get_node();
        RenderObject* render_object = *it;
        ShaderResource* object_shader = render_object->get_current_shader();
        RenderObject::RenderData render_data = render_object->get_render_data();

        if( !object_shader ) object_shader = basic_shader_resource;
        set_current_shader( object_shader );
        
        if( render_data.texture_id ) {
            
            Transform   model_transform;
            Color       modulate;
            Vector2     size_in_pixels;
            model_transform =   (scene_node->get_component<CollisionBody>() )   ?   scene_node->get_transform() 
                                                                                :   scene_node->get_global_transform()  ;
            modulate = render_object->get_global_modulate();
            const float uv_div[4] = { render_data.uv_top_left.x ,render_data.uv_top_left.y , render_data.uv_bottom_right.x ,render_data.uv_bottom_right.y };
            

            GL_CALL( glBindTexture( GL_TEXTURE_2D , render_data.texture_id ) );
            GL_CALL( glUniformMatrix4fv( model_transf_attrib_location , 1 , GL_FALSE , model_transform.m ) );
            GL_CALL( glUniform4fv( uv_div_attrib_location , 1 , uv_div ) );
            GL_CALL( glUniform4f( modulate_attrib_location , ((float)modulate.r)/255.0f , ((float)modulate.g)/255.0f , ((float)modulate.b)/255.0f , ((float)modulate.a)/255.0f ));
            GL_CALL( glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,NULL) );

        }
    }
    
    glfwSwapBuffers( glfw_window );

};


//