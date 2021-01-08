#ifndef COLOR_H
#define COLOR_H

#include <string>

struct Color{
    
    static constexpr const char* class_name = "Color";

    unsigned char r,g,b,a;
    Color( float r , float g , float b , float a=1.0 );
    Color( unsigned char r=255 , unsigned char g=255 , unsigned char b=255 , unsigned char a=255 );
    Color( int color );


    Color   operator*   (const Color  c     ) const ;
    void    operator*=  (const Color  c     );

    operator std::string() const;
    
    static void bind_methods();
};


#endif