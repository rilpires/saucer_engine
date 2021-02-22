#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <string.h>
#include <vector>

namespace saucer_debug{

enum SAUCER_DEBUG_LOG_{
    SAUCER_DEBUG_LOG_INFO,
    SAUCER_DEBUG_LOG_WARN,
    SAUCER_DEBUG_LOG_ERROR
};

extern std::vector<std::ostream*>& extern_console_streams();

// This is necessary since I don't want to other people know my local path I had on __LINE__ when compiled 
std::string saucer_path_format( const char* file );

template<typename T>
void saucer_log__( std::ostream& os , T t  ){
    os << t << std::endl;
}
template<typename T , typename ... Ts>
void saucer_log__( std::ostream& os , T t , Ts ... args ){
    os << t;
    saucer_log__( os , args... );
}
template<typename ... Ts>
void saucer_log_( int level , const char* location , Ts ... args ){
    if      ( level == SAUCER_DEBUG_LOG_INFO )  saucer_log__( std::cout , saucer_path_format(location), "\t[INFO]\t", args... );
    else if ( level == SAUCER_DEBUG_LOG_WARN )  saucer_log__( std::cerr , saucer_path_format(location), "\t[WARNING]\t", args... );
    else if ( level == SAUCER_DEBUG_LOG_ERROR ) saucer_log__( std::cerr , saucer_path_format(location), "\t[ERROR]\t", args... );
    for( auto stream : extern_console_streams() ){
        if      ( level == SAUCER_DEBUG_LOG_INFO )  saucer_log__( *stream , "[INFO]\t", args... );
        else if ( level == SAUCER_DEBUG_LOG_WARN )  saucer_log__( *stream , "[WARNING]\t", args... );
        else if ( level == SAUCER_DEBUG_LOG_ERROR ) saucer_log__( *stream , "[ERROR]\t", args... );
    }
}

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define saucer_print(...) saucer_debug::saucer_log_( saucer_debug::SAUCER_DEBUG_LOG_INFO ,  __FILE__ ":" STRINGIFY(__LINE__) ,  __VA_ARGS__);
#define saucer_warn(...) saucer_debug::saucer_log_( saucer_debug::SAUCER_DEBUG_LOG_WARN ,  __FILE__ ":" STRINGIFY(__LINE__) , __VA_ARGS__);
#define saucer_err(...) saucer_debug::saucer_log_( saucer_debug::SAUCER_DEBUG_LOG_ERROR ,  __FILE__ ":" STRINGIFY(__LINE__) , __VA_ARGS__);
#define saucer_log(...) saucer_print(...);

// Runtime assert. Enabled only if DEBUG is defined
#ifdef DEBUG
#define SAUCER_ASSERT(x,...) ;\
    if( ((bool)(x)) == false ){                 \
        saucer_err("Assertion failed: " , #x , "\n" , __VA_ARGS__ ); \
    }                                           
#else
    #define SAUCER_ASSERT(x,...) ;
#endif


#ifdef DEBUG
#define GL_CALL(x)\
    x ;                                                                                                     \
    {                                                                                                       \
        unsigned int gl_error = glGetError();                                                               \
        while(gl_error != GLEW_OK ){                                                                        \
            saucer_err("[OpenGL Error] Error code " , gl_error , ";\t" , glewGetErrorString(gl_error) );    \
            switch(gl_error){                                                                               \
                case GL_INVALID_ENUM:       saucer_err("GL_INVALID_ENUM");break;                            \
                case GL_INVALID_VALUE:      saucer_err("GL_INVALID_VALUE");break;                           \
                case GL_INVALID_OPERATION:  saucer_err("GL_INVALID_OPERATION");break;                       \
                case GL_OUT_OF_MEMORY:      saucer_err("GL_OUT_OF_MEMORY");break;                           \
            }                                                                                               \
            break;                                                                                          \
        }                                                                                                   \
    }

#define AL_CALL(x)\
    alGetError(); x ;                                                                   \
    {                                                                                   \
        auto al_error = alGetError();                                                   \
        if( al_error != AL_NO_ERROR ){                                                  \
            saucer_err( "[OpenAL Error] Error code " , al_error );                      \
            switch(al_error){                                                           \
                case AL_INVALID_NAME:       saucer_err( "AL_INVALID_NAME" )      break; \
                case AL_INVALID_ENUM:       saucer_err( "AL_INVALID_ENUM" )      break; \
                case AL_INVALID_VALUE:      saucer_err( "AL_INVALID_VALUE" )     break; \
                case AL_INVALID_OPERATION:  saucer_err( "AL_INVALID_OPERATION" ) break; \
                case AL_OUT_OF_MEMORY:      saucer_err( "AL_OUT_OF_MEMORY" )     break; \
            }                                                                                           \
        }                                                                                               \
    }
#define ALC_CALL(device,x)\
    x ;                                                                                     \
    {                                                                                       \
        auto alc_error = alcGetError(device);                                               \
        if( alc_error != ALC_NO_ERROR ){                                                    \
            saucer_err( "[OpenAL Context Error] Context error code " , alc_error );         \
            switch(alc_error){                                                              \
                case ALC_INVALID_DEVICE:    saucer_err( "ALC_INVALID_DEVICE" )     break;   \
                case ALC_INVALID_CONTEXT:   saucer_err( "ALC_INVALID_CONTEXT" )    break;   \
                case ALC_INVALID_ENUM:      saucer_err( "ALC_INVALID_ENUM" )       break;   \
                case ALC_INVALID_VALUE:     saucer_err( "ALC_INVALID_VALUE" )      break;   \
                case ALC_OUT_OF_MEMORY:     saucer_err( "ALC_OUT_OF_MEMORY" )      break;   \
            }                                                                               \
        }                                                                                   \
    }

#else
#define GL_CALL(x) x;                 
#define AL_CALL(x) x;
#define ALC_CALL(device,x) x;                                                    
#endif

}

#endif