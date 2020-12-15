#ifndef COLOR_H
#define COLOR_H

#include <string>

struct Color{
    unsigned char r,g,b,a;
    Color( float r , float g , float b , float a=1.0 );
    Color( unsigned char r , unsigned char g , unsigned char b , unsigned char a=255 );
    Color( int color );
    operator std::string() const;
};

#endif