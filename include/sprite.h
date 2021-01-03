#ifndef SPRITE_H
#define SPRITE_H

#include <unordered_map>

#include "component.h"
#include "scene_node.h"
#include "resources/image.h"

class Sprite : public Component {
    
    REGISTER_AS_COMPONENT(Sprite);

    private:

        ImageResource*  texture;
        Color           modulate;
        Color           self_modulate;
        bool            centralized;
    
    public:
        Sprite();
        ~Sprite();

        ImageResource*  get_texture() const;
        void            set_texture( ImageResource* tex );
        Color           get_modulate() const ;
        void            set_modulate( Color new_col );
        Color           get_self_modulate() const ;
        void            set_self_modulate( Color new_col );
        bool            is_centralized() const ;
        void            set_centralized( bool new_val );

        static void     bind_methods();

};

#endif