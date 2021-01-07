#ifndef ANCHORED_RECT_H
#define ANCHORED_RECT_H

#include "render_object.h"

enum SAUCER_BORDER {
    LEFT_BORDER     ,
    RIGHT_BORDER    ,
    TOP_BORDER      ,
    BOTTOM_BORDER
};

class AnchoredRect : public RenderObject {
    
    REGISTER_AS_INHERITED_COMPONENT(AnchoredRect,RenderObject);
    
    private:
        unsigned char   anchored_borders; // 4 bits mask
        Vector2         rect_pos;
        Vector2         rect_size;
        bool            starts_on_viewport;
        bool            use_scene_node_transform; // Doesn't have any effect if has AnchoredRect parent

    public:
        AnchoredRect();

        virtual RenderData  get_render_data() const ;

        bool                is_border_anchored();
        void                set_anchored_border( int border , bool new_val );
        Vector2             get_rect_pos();
        void                set_rect_pos( Vector2 new_val );
        Vector2             get_rect_size();
        void                set_rect_size( Vector2 new_val );
        bool                get_starts_on_viewport();
        void                set_starts_on_viewport( bool new_val );
        bool                get_use_scene_node_transform();
        void                set_use_scene_node_transform( bool new_val );
        void                grow( int border , float amount );

        static void         bind_methods();


};

#endif