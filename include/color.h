#ifndef COLOR_H
#define COLOR_H

#include <string>

#ifdef SAUCER_EDITOR
#include "imgui.h"
#endif

struct Color{

    struct ColorFloat{ float r,g,b,a;
        Color to_color() const;
    };

    static constexpr const char* class_name = "Color";

    unsigned char r,g,b,a;
    Color( float r , float g , float b , float a=1.0 );
    Color( unsigned char r=255 , unsigned char g=255 , unsigned char b=255 , unsigned char a=255 );
    Color( int r , int g , int b , int a=255 );
    Color( int color );
 
    ColorFloat      to_float() const;

    Color           operator*   (const Color  c     ) const ;
    void            operator*=  (const Color  c     );


    operator std::string() const;
    static void bind_methods();

    #ifdef SAUCER_EDITOR
    operator ImVec4() const { 
        return ImVec4(  float(r)/255.0f,
                        float(g)/255.0f,
                        float(b)/255.0f,
                        float(a)/255.0f); 
    }
    #endif

};


#endif