#include "vector.h"
#include "lua_engine.h"
#include <math.h>


Vector2 Vector2::rotated( float rotation_degrees_cw ) const{
    float rotation_radians_cw = rotation_degrees_cw * M_PI / 180.0;
    float c = cos(rotation_radians_cw);
    float s = sin(rotation_radians_cw);
    return Vector2( x*c - y*s , x*s + y*c );
}

void     Vector2::operator=  (const Vector2  v     )      { x=v.x;y=v.y; }
Vector2  Vector2::operator+  (const Vector2  v     ) const{ return Vector2(x+v.x,y+v.y); }
Vector2  Vector2::operator-  (const Vector2  v     ) const{ return Vector2(x-v.x,y-v.y); }
Vector2  Vector2::operator*  (const Vector2  v     ) const{ return Vector2(x*v.x,y*v.y); }
Vector2  Vector2::operator*  (const float    scale ) const{ return Vector2(x*scale,y*scale); }
Vector2  Vector2::operator/  (const Vector2  v     ) const{ return Vector2(x/v.x,y/v.y); }
Vector2  Vector2::operator/  (const float    div   ) const{ return Vector2(x/div,y/div); }
void     Vector2::operator+= (const Vector2  v     )      { x+=v.x;y+=v.y; }
void     Vector2::operator-= (const Vector2  v     )      { x-=v.x;y-=v.y; }
void     Vector2::operator*= (const Vector2  v     )      { x*=v.x;y*=v.y; }
void     Vector2::operator*= (const float    scale )      { x*=scale;y*=scale; }
void     Vector2::operator/= (const Vector2  v     )      { x/=v.x;y/=v.y; }
void     Vector2::operator/= (const float    div   )      { x/=div;y/=div; }

void Vector2::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION(Vector2,rotated);
}

void     Vector3::operator=  (const Vector3  v     )      { x=v.x;y=v.y;z=v.z; }
Vector3  Vector3::operator+  (const Vector3  v     ) const{ return Vector3(x+v.x,y+v.y,z+v.z); }
Vector3  Vector3::operator-  (const Vector3  v     ) const{ return Vector3(x-v.x,y-v.y,z-v.z); }
Vector3  Vector3::operator*  (const Vector3  v     ) const{ return Vector3(x*v.x,y*v.y,z*v.z); }
Vector3  Vector3::operator*  (const float    scale ) const{ return Vector3(x*scale,y*scale,z*scale); }
Vector3  Vector3::operator/  (const Vector3  v     ) const{ return Vector3(x/v.x,y/v.y,z/v.z); }
Vector3  Vector3::operator/  (const float    div   ) const{ return Vector3(x/div,y/div,z/div); }
void     Vector3::operator+= (const Vector3  v     )      { x+=v.x;y+=v.y;z+=v.z; }
void     Vector3::operator-= (const Vector3  v     )      { x-=v.x;y-=v.y;z-=v.z; }
void     Vector3::operator*= (const Vector3  v     )      { x*=v.x;y*=v.y;z*=v.z; }
void     Vector3::operator*= (const float    scale )      { x*=scale;y*=scale;z*=scale; }
void     Vector3::operator/= (const Vector3  v     )      { x/=v.x;y/=v.y;z/=v.z; }
void     Vector3::operator/= (const float    div   )      { x/=div;y/=div;z/=div; }

void Vector3::bind_methods(){
    
}