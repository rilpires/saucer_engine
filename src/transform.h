#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vector.h"

struct Transform {
    float   m[16];

    Transform();

    Transform&    translate( const Vector2& v );
    Transform&    translate( const Vector3& v );
    Transform&    rotate( const float& r_degrees , Vector3 axis );
    Transform&    rotate_z( const float& r_degrees);
    Transform&    scale( const Vector2& v );
    Transform&    scale( const Vector3& v );

    Transform   operator* ( const Transform& m2 );
};

#endif