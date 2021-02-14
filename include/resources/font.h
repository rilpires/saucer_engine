#ifndef FONT_H
#define FONT_H

#include "resources.h"
#include "resources/image.h"
#include <ft2build.h>
#include FT_FREETYPE_H

class FontResource : public Resource {
    REGISTER_SAUCER_OBJECT(FontResource,Resource)
    private:
        std::vector<unsigned char> raw_data; // Must be allocated during lifetime
        static FT_Library   ft;
        FT_Face             face;
    public:
        struct CharData {
            uint64_t    unicode;
            Vector2     top_left_uv;
            Vector2     bottom_right_uv;
            Vector2     bearing;
            Vector2     pixel_size;
            int         pixels_advance;
        };

    protected:
        std::unordered_map<uint64_t, FontResource::CharData > char_datas;
        int         max_pixels_height;
        TextureId   tex_id;
        

    public:
        FontResource( const std::vector<uint8_t>& data );
        ~FontResource();

        
        const FontResource::CharData    get_char_data( uint64_t unicode ) const;      
        TextureId                       get_texture_id() const;
        int                             get_max_pixels_height() const;

        static void                     initialize();
        static void                     bind_methods();

};


#endif