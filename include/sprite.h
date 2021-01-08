#ifndef SPRITE_H
#define SPRITE_H

#include "render_object.h"

class TextureResource;
class Sprite : public RenderObject {
    
    REGISTER_AS_INHERITED_COMPONENT(Sprite,RenderObject);

    private:

        TextureResource*    texture;
        short               h_frames;
        short               v_frames;
        short               frame_index;
        bool                centralized;
    
    public:
        Sprite();
        ~Sprite();

        std::vector<RenderData>  generate_render_data() const ;

        TextureResource*  get_texture() const;
        void            set_texture( TextureResource* tex );
        short           get_h_frames() const;
        void            set_h_frames( short new_val );
        short           get_v_frames() const;
        void            set_v_frames( short new_val );
        short           get_frame_index() const;
        void            set_frame_index( short new_val );
        bool            is_centralized() const ;
        void            set_centralized( bool new_val );

        static void     bind_methods();

};

#endif