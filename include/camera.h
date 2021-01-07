#ifndef CAMERA_H
#define CAMERA_H

#include "component.h"

class Camera : public Component {
    REGISTER_AS_COMPONENT(Camera);
    
    private:
        Vector2 zoom;
        float   left_limit;
        float   right_limit;
        float   top_limit;
        float   bottom_limit;

    public:
        Camera();
        ~Camera();

        Vector2     get_zoom() const;
        void        set_zoom( Vector2 new_val );
        float       get_left_limit() const;
        void        set_left_limit( float new_val );
        float       get_right_limit() const;
        void        set_right_limit( float new_val );
        float       get_top_limit() const;
        void        set_top_limit( float new_val );
        float       get_bottom_limit() const;
        void        set_bottom_limit( float new_val );
        bool        is_active() const ;
        void        set_active( bool new_val);
        void        entered_tree();
        void        exiting_tree();

        static void bind_methods();


};

#endif