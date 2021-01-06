#include "transform.h"
#include <math.h>
#include <iostream>

Transform::Transform(){
    // Identity transform
    for( int i = 0 ; i < 16 ; i++ ) m[i] = ((i%5)==0)?(1):(0);
}

// https://stackoverflow.com/a/1148405/10508429
// @shoosh said this was copied from MESA(https://www.mesa3d.org/) implementation of GLU library
Transform       Transform::inverted(){
    Transform ret;
    float det;
    
    ret.m[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    ret.m[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    ret.m[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    ret.m[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    ret.m[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    ret.m[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    ret.m[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    ret.m[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    ret.m[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    ret.m[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    ret.m[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    ret.m[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    ret.m[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    ret.m[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    ret.m[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    ret.m[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * ret.m[0] + m[1] * ret.m[4] + m[2] * ret.m[8] + m[3] * ret.m[12];

    if (det == 0) {
        std::cerr << "[WARNING] Couldn't invert a transform matrix... Unexpected behavior ahead maybe?" << std::endl;
        det = 0.000001;
    }

    det = 1.0 / det;
    for (int i = 0; i < 16; i++) ret.m[i] *= det;

    return ret;
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

Transform Transform::operator* ( const Transform& m2 ) const {
    const Transform& m1 = *this ;
    Transform ret;
    
    for( int i = 0 ; i < 4 ; i++ )
    for( int j = 0 ; j < 4 ; j++ )
        ret.m[j+4*i] =   m1.m[4*i + 0] * m2.m[4*0 + j] 
                       + m1.m[4*i + 1] * m2.m[4*1 + j] 
                       + m1.m[4*i + 2] * m2.m[4*2 + j] 
                       + m1.m[4*i + 3] * m2.m[4*3 + j] ;
    return ret;
}

Vector2     Transform::operator* ( const Vector2& v ) const {
    //  [ m0   m1   m2   m3   ] [ v0 ]   
    //  [ m4   m5   m6   m7   ] [ v1 ]   
    //  [ m8   m9   m10  m11  ] [ v2 ]  -> v2 will be ignored
    //  [ m12  m13  m14  m15  ] [ v3 ]  
    Vector2 ret;
    ret.x = m[0]*v.x + m[1]*v.y + m[3]*1;
    ret.y = m[4]*v.x + m[5]*v.y + m[7]*1;
    return ret; 
}
Vector3     Transform::operator* ( const Vector3& v ) const {
    //  [ m0   m1   m2   m3   ] [ v0 ]   
    //  [ m4   m5   m6   m7   ] [ v1 ]   
    //  [ m8   m9   m10  m11  ] [ v2 ]  
    //  [ m12  m13  m14  m15  ] [ v3 ]  
    Vector3 ret;
    ret.x = m[0]*v.x + m[1]*v.y + m[2]*v.z  + m[3]*1;
    ret.y = m[4]*v.x + m[5]*v.y + m[6]*v.z  + m[7]*1;
    ret.z = m[8]*v.x + m[9]*v.y + m[10]*v.z + m[11]*1;
    return ret; 
}

void Transform::bind_methods(){
    
}