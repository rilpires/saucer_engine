#include "window.h"

#include <queue>
#include <algorithm>

#include "scene_node.h"
#include "input.h"


void setupShader( unsigned int program , GLenum shader_type , std::string src ){

    int src_size = src.size();
    const char* src_c_str = src.c_str();

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
    GL_CALL( glAttachShader( program , shader ); )
    GL_CALL( glDeleteShader( shader ); )
}


void    Window::handle_inputs(){
    if( current_scene ){
        Input::InputEvent* next_input_event = input->pop_event_queue();
        while(next_input_event){
            current_scene->propagate_input_event( next_input_event );
            delete next_input_event;   
            next_input_event = input->pop_event_queue();
        }
    } else while( input->pop_event_queue() );
}
void    Window::setup_renderer() const {
    // Compiling & Linking shaders
    std::string vs = read_file_as_string( "res/shaders/basic_vertex.vs" );
    std::string fs = read_file_as_string( "res/shaders/basic_fragment.fs" );

    unsigned int program = glCreateProgram();
    setupShader( program , GL_VERTEX_SHADER , vs );
    setupShader( program , GL_FRAGMENT_SHADER , fs );
    GL_CALL( glBindFragDataLocation( program, 0, "outColor") ); 
    GL_CALL( glLinkProgram( program ) );
    GL_CALL( glUseProgram( program ) );
    
    // VAO (Vertex Array Object)
    unsigned int vao;
    GL_CALL( glCreateVertexArrays(1,&vao) );
    GL_CALL( glBindVertexArray(vao) );

    Vector2 window_size = get_window_size();
    // VBO (Vertex Buffer Object)
    float vertex_data[] = {
    /*  X                    Y                      Z       U     V    */
        -0.5f*window_size.x , -0.5f*window_size.y ,   0.0 ,   0.0 , 1.0 ,
        +0.5f*window_size.x , -0.5f*window_size.y ,   0.0 ,   1.0 , 1.0 ,
        +0.5f*window_size.x , +0.5f*window_size.y ,   0.0 ,   1.0 , 0.0 ,
        -0.5f*window_size.x , +0.5f*window_size.y ,   0.0 ,   0.0 , 0.0
    };
    unsigned int vbo;
    GL_CALL( glGenBuffers(1, &vbo) );
    GL_CALL( glBindBuffer(GL_ARRAY_BUFFER,vbo) );
    GL_CALL( glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) , vertex_data , GL_STATIC_DRAW ) );

    // VEB (Vertex Element Buffer )
    unsigned int vertex_index[] = {
        0 , 2 , 1 ,
        0 , 3 , 2
    };

    GL_CALL( glEnable(GL_DEPTH_TEST) );
    GL_CALL( glEnable(GL_BLEND) );
    GL_CALL( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );  
    GL_CALL( glDepthFunc(GL_LEQUAL) );

    unsigned int veb;
    GL_CALL( glGenBuffers(1,&veb) );
    GL_CALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,veb) );
    GL_CALL( glBufferData(GL_ELEMENT_ARRAY_BUFFER , sizeof(vertex_index) , vertex_index , GL_STATIC_DRAW) );
    GL_CALL( glVertexAttribPointer( 0 , 3 , GL_FLOAT , false , 5*sizeof(GL_FLOAT) , 0 ));
    GL_CALL( glVertexAttribPointer( 1 , 2 , GL_FLOAT , false , 5*sizeof(GL_FLOAT) , (void*)(3*sizeof(GL_FLOAT)) ));
    GL_CALL( glEnableVertexAttribArray(  0 ) );
    GL_CALL( glEnableVertexAttribArray( 1 ) );
}
void    Window::render_scene() const {

    GL_CALL( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
    GL_CALL( glClearColor( 0,0,input->is_mouse_button_pressed(BUTTON_LEFT),1 ) );
    
    Vector2 window_size = get_window_size();
    Transform camera_transf = Transform();
    if( current_scene && current_scene->get_current_camera() )
        camera_transf = current_scene->get_current_camera()->get_global_transform();
    
    unsigned int program_location = 1U;
    GL_CALL( unsigned int viewport_size_attrib_location = glGetUniformLocation(program_location,"viewport_size") );
    GL_CALL( unsigned int model_transf_attrib_location  = glGetUniformLocation(program_location,"model_transf") );
    GL_CALL( unsigned int camera_transf_attrib_location = glGetUniformLocation(program_location,"camera_transf") );
    
    GL_CALL( glUniform2f( viewport_size_attrib_location , window_size.x , window_size.y ) );
    GL_CALL( glUniformMatrix4fv( camera_transf_attrib_location , 1 , GL_FALSE , camera_transf.m ) );

    std::vector<SceneNode*> nodes;
    std::queue<SceneNode*> nodes_queue;
    
    nodes_queue.push( get_current_scene()->get_root_node() );
    while( nodes_queue.size() ){
        SceneNode* scene_node = nodes_queue.front();
        if( scene_node->get_image_texture() ) nodes.push_back( scene_node );
        nodes_queue.pop();
        
        
        for( auto child : scene_node->get_children() )
            nodes_queue.push( child );

    }
    
    // Z-sorting
    std::sort( nodes.begin() , nodes.end() , [](const SceneNode* n1 , const SceneNode* n2)->bool{
        return n2->get_global_z() > n1->get_global_z();
    });

    for( auto it = nodes.begin() ; it != nodes.end() ; it ++ ){
        SceneNode* scene_node = *it;
        Transform model_transform = scene_node->get_global_transform();
        model_transform.scale(Vector3(1,1,-1));

        GL_CALL( glBindTexture( GL_TEXTURE_2D , scene_node->get_image_texture()->get_texture_id() ) );
        GL_CALL( glUniformMatrix4fv( model_transf_attrib_location , 1 , GL_FALSE , model_transform.m ) );
        GL_CALL( glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,NULL) );
    }

}


Window::Window( Vector2 p_size , Vector2 p_pos , const char* title ){
    
    if( !glfwInit() ) std::cerr << "Failed to glfwInit()" << std::endl;
    
    glfw_window = glfwCreateWindow( p_size.x , p_size.y , title , NULL , NULL );
    
    // Positioning
    glfwSetWindowPos( glfw_window , p_pos.x , p_pos.y );

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


    // Setting up initial stuffs for OpenGL state:
    setup_renderer();

    // Setting input callbacks
    glfwSetCursorPosCallback( glfw_window , Input::mouse_pos_callback );
    glfwSetKeyCallback( glfw_window , Input::key_callback );
    glfwSetMouseButtonCallback( glfw_window , Input::mouse_button_callback );

    // Initial scene is Null
    current_scene = NULL;
    input = Input::instance();

}

Window::~Window(){
    std::cout << "Closing window" << std::endl;
    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}

void        Window::update(){
    glfwPollEvents();
    handle_inputs();
    render_scene();
    glfwSwapBuffers(glfw_window);
}


void        Window::set_window_size( Vector2 new_size ){
    glfwSetWindowSize( glfw_window , new_size.x , new_size.y );
}
Vector2     Window::get_window_size() const {
    int w,h;
    glfwGetWindowSize( glfw_window , &w , &h );
    return Vector2( w,h );
}
void        Window::set_window_pos( Vector2 new_pos ){
    glfwSetWindowPos( glfw_window , new_pos.x , new_pos.y );
}
Vector2     Window::get_window_pos() const {
    int x,y;
    glfwGetWindowPos( glfw_window , &x , &y );
    return Vector2(x,y);
}
void        Window::set_fullscreen( bool fs ){
    if( !(this->is_fullscreen()) && fs ){
        int w,h;
        glfwGetWindowSize( glfw_window , &w , &h );
        glfwSetWindowMonitor( glfw_window , glfwGetPrimaryMonitor() , 0 , 0 , w , h , 60 );
    } else if( this->is_fullscreen() && !fs ) {
        int w,h;
        glfwGetWindowSize( glfw_window , &w , &h );
        glfwSetWindowMonitor( glfw_window , NULL , 0 , 0 , w , h , 60 );
    }
}
bool        Window::is_fullscreen(){
    return glfwGetWindowMonitor(glfw_window) != NULL;
}
bool        Window::should_close() const{
    return glfwWindowShouldClose(glfw_window);
}
