#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <string.h>

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define saucer_print(...) saucer_log_<std::cout>(__VA_ARGS__) << std::endl;
#define saucer_log(...) saucer_log_<std::cout>( __FILE__ ":" STRINGIFY(__LINE__) , ":\t[LOG]\t" , __VA_ARGS__) << std::endl;
#define saucer_err(...) saucer_log_<std::cerr>( __FILE__ ":" STRINGIFY(__LINE__) , ":\t[ERROR]\t" , __VA_ARGS__) << std::endl;
#define saucer_warn(...) saucer_log_<std::cerr>( __FILE__ ":" STRINGIFY(__LINE__) , ":\t[WARNING]\t" , __VA_ARGS__) << std::endl;

template< std::ostream& stream ,  typename T , class = typename std::enable_if<std::is_arithmetic<T>::value>::type >
std::ostream& saucer_log_( T t ){
    return stream << t;
}
template< std::ostream& stream , typename T , class = typename std::enable_if<!std::is_arithmetic<T>::value>::type , class=void >
std::ostream& saucer_log_( T t ){
    return stream << std::string(t);
}
template< std::ostream& stream , class = void , class=void >
std::ostream& saucer_log_( const unsigned char* t ){
    return stream << std::string((char*)t);
}
template< std::ostream& stream , typename T , typename ... Ts >
std::ostream& saucer_log_( T t , Ts ... args ){
    saucer_log_<stream,T>(t) << " ";
    return saucer_log_<stream,Ts...>( args... );
}

#ifdef DEBUG
#define GL_CALL(x)\
    x ;                                                                                                     \
    {                                                                                                       \
        unsigned int gl_error = glGetError();                                                               \
        while(gl_error != GLEW_OK ){                                                                        \
            saucer_err("[OpenGL Error] Error code " , gl_error , ";\t" , glewGetErrorString(gl_error) );    \
            if (gl_error == GL_INVALID_OPERATION ) break;                                                   \
        }                                                                                                   \
    }
#else
#define GL_CALL(x)\
    x;                                                                 
#endif

#endif