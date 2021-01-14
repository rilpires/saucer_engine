#ifndef PATCH_RECT_H
#define PATCH_RECT_H

#include "anchored_rect.h"

class PatchRect : public AnchoredRect {
    
    REGISTER_AS_INHERITED_COMPONENT(PatchRect,AnchoredRect);
    
    private:
        short               margins[4]; // One for each border
        bool                draw_center;
        TextureResource*    texture;
        Vector2             top_left_region;
        Vector2             bottom_right_region;

    public:
        PatchRect();
        ~PatchRect();

        std::vector<RenderData>  generate_render_data() ;

        short               get_margin( int border ) const;
        void                set_margin( int border , short new_val );
        TextureResource*    get_texture() const ;
        void                set_texture( TextureResource* tex );
        void                set_draw_center( bool new_val);
        bool                get_draw_center() const;
        Vector2             get_top_left_region() const;
        void                set_top_left_region(Vector2 new_val);
        Vector2             get_bottom_right_region() const;
        void                set_bottom_right_region(Vector2 new_val);

        static void         bind_methods();

};

#endif