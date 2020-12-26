#ifndef VECTOR_H
#define VECTOR_H

#include <string>

class Transform;

struct Vector2 {
    float x,y;

    Vector2( float p_x=0 , float p_y=0 ) : x(p_x) , y(p_y) {}

    Vector2 rotated( float rotation_degrees ) const ;

    void operator=( const Vector2& v );
    Vector2 operator+( const Vector2& v ) const;
    Vector2 operator-( const Vector2& v ) const;
    Vector2 operator*( const Vector2 v ) const;
    Vector2 operator*( const float scale ) const;
    Vector2 operator/( const Vector2 v ) const;
    Vector2 operator/( const float div ) const;
    Vector2 operator+=( const Vector2& v );
    Vector2 operator-=( const Vector2& v );
    Vector2 operator*=( const Vector2& v );
    Vector2 operator*=( const float scale );
    Vector2 operator/=( const Vector2& v );
    Vector2 operator/=( const float div );

    operator std::string() const{
        char buff[32];
        sprintf(buff , "Vector2( %f , %f )" , x , y );
        return std::string(buff);
    }
};

struct Vector3 {
    float x,y,z;

    Vector3( float p_x , float p_y , float p_z ) : x(p_x) , y(p_y) , z(p_z){}

};

#endif