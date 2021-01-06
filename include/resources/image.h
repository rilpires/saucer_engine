#ifndef IMAGE_RESOURCE_H
#define IMAGE_RESOURCE_H

#include "resources.h"
#include "color.h"

typedef unsigned int TextureId;

class ImageResource : public Resource {
    REGISTER_SAUCER_OBJECT(ImageResource,Resource)
    
    friend class ResourceManager;
    private:
        unsigned char*          data;
        size_t                  width,height;
        TextureId               tex_id;
        ImageResource( std::string filepath );
        ~ImageResource();
    public:
        TextureId               get_texture_id() const {return tex_id;}
        Color                   get_pixel(size_t x , size_t y) const;
        Vector2                 get_size() const {return Vector2(width,height);};
        
        static void             bind_methods();
};

#endif
