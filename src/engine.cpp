#include "engine.h"
#include "core.h"

#define FPS_FRAMES_TO_ACCOUNT 30

GLFWwindow*         Engine::glfw_window     = nullptr;
Scene*              Engine::current_scene   = nullptr;
std::list<double>   Engine::last_uptimes;

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


void    Engine::setup_renderer(){
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


void Engine::initialize( Vector2 p_size , Vector2 p_pos , const char* title ){
    
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

}

void Engine::close(){
    std::cout << "Closing window" << std::endl;
    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}

void        Engine::update(){
    last_uptimes.push_front( get_uptime() );
    if( last_uptimes.size() > FPS_FRAMES_TO_ACCOUNT ) {
        last_uptimes.pop_back();
    }

    glfwPollEvents();
    if(current_scene) {
        current_scene->loop();
    }
    glfwSwapBuffers(glfw_window);
}
double      Engine::get_uptime(){
    return glfwGetTime();
}
double      Engine::get_fps(){
    size_t number_of_frames = last_uptimes.size();
    double elapsed_time = last_uptimes.front() - last_uptimes.back();
    if( number_of_frames > 1 && elapsed_time > 0 ){
        return (number_of_frames-1) / elapsed_time; 
    } else return 0;
}
double      Engine::get_last_frame_duration(){
    if( last_uptimes.size() >= 2 ){
        auto begin = last_uptimes.begin();
        double ret = *(begin) - *(begin++);
        return ret;
    }
    else return 0.0;
}
void        Engine::set_current_scene(Scene* scene){
    current_scene = scene;
}
Scene*      Engine::get_current_scene(){ 
    return current_scene;
}
void        Engine::set_window_size( Vector2 new_size ){
    glfwSetWindowSize( glfw_window , new_size.x , new_size.y );
}
Vector2     Engine::get_window_size(){
    int w,h;
    glfwGetWindowSize( glfw_window , &w , &h );
    return Vector2( w,h );
}
void        Engine::set_window_pos( Vector2 new_pos ){
    glfwSetWindowPos( glfw_window , new_pos.x , new_pos.y );
}
Vector2     Engine::get_window_pos() {
    int x,y;
    glfwGetWindowPos( glfw_window , &x , &y );
    return Vector2(x,y);
}
void        Engine::set_fullscreen( bool fs ){
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
bool        Engine::is_fullscreen(){
    return glfwGetWindowMonitor(glfw_window) != NULL;
}
bool        Engine::should_close() {
    return glfwWindowShouldClose(glfw_window);
}
void        Engine::bind_methods(){
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_uptime );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_fps );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_current_scene );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_current_scene );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_window_size );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_window_size );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_window_pos );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , get_window_pos );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , set_fullscreen );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Engine , is_fullscreen );
}