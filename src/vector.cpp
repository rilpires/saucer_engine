#include "vector.h"
#include <math.h>


Vector2 Vector2::rotated( float rotation_degrees_cw ) const{
    float rotation_radians_cw = rotation_degrees_cw * M_PI / 180.0;
    float c = cos(rotation_radians_cw);
    float s = sin(rotation_radians_cw);
    return Vector2( x*c - y*s , x*s + y*c );
}

void Vector2::operator=( const Vector2& v ){
    x = v.x ; y = v.y;
}
Vector2 Vector2::operator+( const Vector2& v ) const{
    Vector2 ret = Vector2( x+v.x , y+v.y );
    return ret;
}
Vector2 Vector2::operator-( const Vector2& v ) const {
    Vector2 ret = Vector2( x-v.x , y-v.y );
    return ret;
}
Vector2 Vector2::operator*( const Vector2 v ) const {
    return Vector2(x*v.x,y*v.y);
}
Vector2 Vector2::operator*( const float scale ) const {
    return Vector2(x*scale,y*scale);
}
Vector2 Vector2::operator/( const Vector2 v ) const {
    return Vector2(x/v.x,y/v.y);
}
Vector2 Vector2::operator/( const float div ) const {
    return Vector2(x/div,y/div);
}


Vector2 Vector2::operator+=( const Vector2& v ){
    x += v.x; y += v.y;
    return *this;
}
Vector2 Vector2::operator-=( const Vector2& v ){
    x -= v.x; y -= v.y;
    return *this;
}
Vector2 Vector2::operator*=( const Vector2& v ){
    x *= v.x; y *= v.y;
    return *this;
}
Vector2 Vector2::operator*=( const float scale ){
    x *= scale; y *= scale;
    return *this;
}
Vector2 Vector2::operator/=( const Vector2& v ){
    x /= v.x; y /= v.y;
    return *this;
}
Vector2 Vector2::operator/=( const float div ){
    x /= div; y /= div;
    return *this;
}

