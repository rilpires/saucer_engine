#ifndef VECTOR_H
#define VECTOR_H

#include <string>

class Transform;

struct Vector2 {

    static constexpr const char* class_name = "Vector2";

    float x,y;
    
    Vector2( float p_x=0 , float p_y=0 ) : x(p_x) , y(p_y) {}

    Vector2 rotated( const float rotation_degrees ) const ;

    void     operator=  (const Vector2  v     )       ;
    Vector2  operator+  (const Vector2  v     ) const ;
    Vector2  operator-  (const Vector2  v     ) const ;
    Vector2  operator*  (const Vector2  v     ) const ;
    Vector2  operator*  (const float    scale ) const ;
    Vector2  operator/  (const Vector2  v     ) const ;
    Vector2  operator/  (const float    div   ) const ;
    void     operator+= (const Vector2  v     )       ;
    void     operator-= (const Vector2  v     )       ;
    void     operator*= (const Vector2  v     )       ;
    void     operator*= (const float    scale )       ;
    void     operator/= (const Vector2  v     )       ;
    void     operator/= (const float    div   )       ;

    operator std::string() const{
        char buff[48];
        sprintf(buff , "Vector2( %.4f , %.4f )" , x , y );
        return std::string(buff);
    }

    static void bind_methods();

};

struct Vector3 {
    
    static constexpr const char* class_name = "Vector3";

    float x,y,z;
    
    Vector3( float p_x=0 , float p_y=0 , float p_z=0 ) : x(p_x) , y(p_y) , z(p_z){};

    
    void     operator=  (const Vector3  v     )       ;
    Vector3  operator+  (const Vector3  v     ) const ;
    Vector3  operator-  (const Vector3  v     ) const ;
    Vector3  operator*  (const Vector3  v     ) const ;
    Vector3  operator*  (const float    scale ) const ;
    Vector3  operator/  (const Vector3  v     ) const ;
    Vector3  operator/  (const float    div   ) const ;
    void     operator+= (const Vector3  v     )       ;
    void     operator-= (const Vector3  v     )       ;
    void     operator*= (const Vector3  v     )       ;
    void     operator*= (const float    scale )       ;
    void     operator/= (const Vector3  v     )       ;
    void     operator/= (const float    div   )       ;

    operator std::string() const{
        char buff[48];
        sprintf(buff , "Vector3( %.4f , %.4f , %.4f )" , x , y , z );
        return std::string(buff);
    }

    static void bind_methods();
};

#endif