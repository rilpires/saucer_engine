#include "transform.h"
#include <math.h>

Transform::Transform(){
    // Identity transform
    for( int i = 0 ; i < 16 ; i++ ) m[i] = ((i%5)==0)?(1):(0);
}



Transform&    Transform::translate( const Vector2& v ){
    return translate(Vector3(v.x,v.y,0.0));
}
Transform&    Transform::translate( const Vector3& v ){
    m[4*0 + 3] += v.x;
    m[4*1 + 3] += v.y;
    m[4*2 + 3] += v.z;
    return *this;
}
Transform&    Transform::rotate( const float& r_degrees , Vector3 axis ){
    double radians = r_degrees * M_PI / 180.0;
    double s = sin(radians);
    double c = cos(radians);
    float& x = axis.x;
    float& y = axis.y;
    float& z = axis.z; 
    Transform rotation_matrix;
    rotation_matrix.m[0] = c + x*x*(1-c);
    rotation_matrix.m[1] = x*y*(1-c)-z*s;
    rotation_matrix.m[2] = x*z*(1-c)+y*s;
    rotation_matrix.m[4] = y*x*(1-c)+z*s;
    rotation_matrix.m[5] = c+y*y*(1-c);
    rotation_matrix.m[6] = y*z*(1-c)-x*s;
    rotation_matrix.m[8] = z*x*(1-c)-y*s;
    rotation_matrix.m[9] = z*y*(1-c)+x*s;
    rotation_matrix.m[10] = c+z*z*(1-c);
    (*this) = rotation_matrix*(*this);
    return *this;
}
Transform&    Transform::rotate_z( const float& r_degrees){
    double radians = r_degrees * M_PI / 180.0;
    double s = sin(radians);
    double c = cos(radians);
    Transform rotation_matrix;
    rotation_matrix.m[0] = c;
    rotation_matrix.m[1] = -s;
    rotation_matrix.m[4] = s;
    rotation_matrix.m[5] = c;
    (*this) = rotation_matrix*(*this);
    return *this;
}
Transform&    Transform::scale( const Vector2& v ){
    return scale(Vector3(v.x,v.y,1.0));
}
Transform&    Transform::scale( const Vector3& v ){
    m[4*0 + 0] *= v.x; m[4*0 + 1] *= v.x; m[4*0 + 2] *= v.x; ; m[4*0 + 3] *= v.x;
    m[4*1 + 0] *= v.y; m[4*1 + 1] *= v.y; m[4*1 + 2] *= v.y; ; m[4*1 + 3] *= v.y;
    m[4*2 + 0] *= v.z; m[4*2 + 1] *= v.z; m[4*2 + 2] *= v.z; ; m[4*2 + 3] *= v.z;
    return *this;
}

Transform Transform::operator* ( const Transform& m2 ){
    Transform& m1 = *this ;
    Transform ret;
    // m1*m2 = m1.m2
    for( int i = 0 ; i < 4 ; i++ )
    for( int j = 0 ; j < 4 ; j++ )
        ret.m[j+4*i] =   m1.m[4*i + 0] * m2.m[4*0 + j] 
                       + m1.m[4*i + 1] * m2.m[4*1 + j] 
                       + m1.m[4*i + 2] * m2.m[4*2 + j] 
                       + m1.m[4*i + 3] * m2.m[4*3 + j] ;
    return ret;
}

void Transform::bind_methods(){
    
}