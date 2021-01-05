#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#define GL_CALL(x)\
    x ;                                                                         \
    {                                                                           \
        unsigned int gl_error = glGetError();                                   \
        while(gl_error != GLEW_OK ){                                            \
            std::cerr << "[OpenGL Error] Error code " << gl_error << std::endl  \
            << glewGetErrorString(gl_error) << std::endl                        \
            << __FILE__ << "(" << __LINE__ << ")" << std::endl;                 \
            if (gl_error == GL_INVALID_OPERATION ) break;                       \
        }                                                                       \
    }
#else
#define GL_CALL(x)\
    x;                                                                 
#endif

#endif