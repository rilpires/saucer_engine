#include "color.h"

Color::Color( float r , float g , float b , float a ){
    this->r = r * 255U;
    this->g = g * 255U;
    this->b = b * 255U;
    this->a = a * 255U;
}
Color::Color( unsigned char r , unsigned char g , unsigned char b , unsigned char a ){
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}
Color::Color( int color ){
    this->r = (color >> 24) | 255;
    this->g = (color >> 16) | 255;
    this->b = (color >> 8)  | 255;
    this->a = (color >> 0)  | 255;
}
Color::operator std::string() const{
    char buff[32];
    sprintf(buff,"(%d, %d, %d)",r,g,b);
    return std::string(buff);
}