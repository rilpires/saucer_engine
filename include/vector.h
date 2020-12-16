#ifndef VECTOR_H
#define VECTOR_H

class Transform;

struct Vector2 {
    float x,y;

    Vector2( float p_x=0 , float p_y=0 ) : x(p_x) , y(p_y) {}

    Vector2 rotated( float rotation_degrees ) const ;

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
};

struct Vector3 {
    float x,y,z;

    Vector3( float p_x , float p_y , float p_z ) : x(p_x) , y(p_y) , z(p_z){}

};

#endif