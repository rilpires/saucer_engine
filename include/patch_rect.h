#ifndef PATCH_RECT_H
#define PATCH_RECT_H

#include "anchored_rect.h"

class PatchRect : public AnchoredRect {
    
    REGISTER_AS_INHERITED_COMPONENT(PatchRect,AnchoredRect);
    
    private:
        short               margins[4]; // One for each border
        bool                draw_center;
        TextureResource*    texture;
        Rect                region;

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
        Rect                get_region() const;
        void                set_region(Rect new_val);
        
        static void         bind_methods();
        void                push_editor_items();
        YamlNode            to_yaml_node() const override ;
        void                from_yaml_node( YamlNode ) override ;

};

#endif