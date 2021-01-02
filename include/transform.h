#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vector.h"

struct Transform {
    
    static constexpr const char* class_name = "Transform";

    float   m[16];

    Transform();

    Transform&  translate( const Vector2& v );
    Transform&  translate( const Vector3& v );
    Transform&  rotate( const float& r_degrees , Vector3 axis );
    Transform&  rotate_z( const float& r_degrees);
    Transform&  scale( const Vector2& v );
    Transform&  scale( const Vector3& v );

    Transform   operator* ( const Transform& m2 ) const ;
    
    Vector2     operator* ( const Vector2& v ) const ;
    Vector3     operator* ( const Vector3& v ) const ;

    static void bind_methods();
};

#endif