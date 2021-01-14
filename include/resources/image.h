#ifndef IMAGE_RESOURCE_H
#define IMAGE_RESOURCE_H

#include "resources.h"
#include "color.h"

typedef unsigned int TextureId;

enum TEXTURE_WRAP_MODE {
    REPEAT,
    CLAMP,
    MIRRORED_REPEAT,
    ALPHA_ZERO
};

class TextureResource : public Resource {
    REGISTER_SAUCER_OBJECT(TextureResource,Resource)
    
    private:
        unsigned char*          data;
        short                   wrap_mode;
        size_t                  width,height;
        TextureId               tex_id;
        ~TextureResource();
    public:
        TextureResource( std::string filepath );
        TextureId               get_texture_id() const {return tex_id;}
        Color                   get_pixel(size_t x , size_t y) const;
        Vector2                 get_size() const;
        int                     get_wrap_mode() const;
        void                    set_wrap_mode( int new_mode );
        unsigned char*          get_data() const;

        static void             bind_methods();
};

#endif
