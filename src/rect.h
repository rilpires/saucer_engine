#ifndef RECT_H
#define RECT_H

#include "vector.h"
#include <string>

struct Rect {
    Vector2 top_left;
    Vector2 bottom_right;

    Rect( float x1=0.0f , float y1=0.0f , float x2=0.0f , float y2=0.0f );
    Rect( Vector2 p_top_left , Vector2 p_bottom_right );

    Rect        rect_intersection(Rect) const;
    Rect        rect_union(Rect) const;
    Vector2     get_size() const;
    float       get_area() const;
    float       get_width() const;
    float       get_height() const;
    bool        is_point_inside( Vector2 p ) const;

    bool operator==( const Rect& r ) const;
    operator std::string() const;

    static void bind_methods();

};

#endif