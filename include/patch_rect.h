#ifndef PATCH_RECT_H
#define PATCH_RECT_H

#include "anchored_rect.h"

class PatchRect : public AnchoredRect {
    
    REGISTER_AS_INHERITED_COMPONENT(PatchRect,AnchoredRect);
    
    private:
        short               margins[4]; // One for each border
        bool                draw_center;
        TextureResource*    texture;

    public:
        PatchRect();

        std::vector<RenderData>  generate_render_data() const ;

        short               get_margin( int border ) const;
        void                set_margin( int border , short new_val );
        TextureResource*    get_texture() const ;
        void                set_texture( TextureResource* tex );
        void                set_draw_center( bool new_val);
        bool                get_draw_center() const;
        
        static void         bind_methods();

};

#endif