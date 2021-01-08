#include "resources/image.h"
#include "core.h"

#include <png.h>
#include <string.h>
#include "debug.h"


TextureResource::TextureResource( std::string filepath ) : Resource(filepath) {

    png_image image; /* The control structure used by libpng */

    /* Initialize the 'png_image' structure. */
    memset(&image, 0, (sizeof image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_file(&image,filepath.c_str()) != 0)
    {
        image.format = PNG_FORMAT_RGBA;
        saucer_print( "image size: " , PNG_IMAGE_SIZE(image) )
        data = new unsigned char[PNG_IMAGE_SIZE(image)];
        width = image.width;
        height = image.height;

        if (data == NULL || png_image_finish_read(&image, NULL/*background*/, data, 0/*row_stride*/, NULL/*colormap*/) == 0){
            png_image_free(&image);
            saucer_err( "Error loading image from " , filepath )
        }
        else{
            GLuint old_binded_tex = Engine::get_render_engine()->get_last_used_texture();
            GL_CALL( glGenTextures(1,&tex_id) );
            GL_CALL( glBindTexture(GL_TEXTURE_2D,tex_id) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_NEAREST ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_NEAREST ) );
            GL_CALL( glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data) );
            GL_CALL( glBindTexture(GL_TEXTURE_2D,old_binded_tex) );
            set_wrap_mode( CLAMP );
            return;
        }
    }
    else saucer_err( "Error loading image from " , filepath )    
}
TextureResource::~TextureResource(){
    delete[] data;
    GL_CALL( glDeleteTextures(1,&tex_id) );
}
Color           TextureResource::get_pixel(size_t x , size_t y) const{
    size_t r_pointer = 4*( x + y*width );
    return Color( data[r_pointer] , data[r_pointer+1] , data[r_pointer+2] , data[r_pointer+3] );
}
Vector2           TextureResource::get_size() const{
    return Vector2(width,height);
}
int  TextureResource::get_wrap_mode() const{
    return wrap_mode;
}
void TextureResource::set_wrap_mode( int new_mode ){
    if( wrap_mode == new_mode ) return;
    wrap_mode = new_mode;
    GLuint old_binded_tex = Engine::get_render_engine()->get_last_used_texture();
    GL_CALL( glBindTexture(GL_TEXTURE_2D,tex_id) );
    switch (wrap_mode){
        case REPEAT:{
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_REPEAT ) );
            break;
        }
        case CLAMP:{
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE ) );
            break;
        }
        case MIRRORED_REPEAT:{
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_MIRRORED_REPEAT ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_MIRRORED_REPEAT ) );
            break;
        }
        case ALPHA_ZERO:{
            const float c[] = {0.0f,0.0f,0.0f,0.0f};
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_CLAMP_TO_BORDER ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_CLAMP_TO_BORDER ) );
            GL_CALL( glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR , c ) );
            break;
        }
        default:
            saucer_err("Invalid wrap mode for texture.");
    }
    GL_CALL( glBindTexture(GL_TEXTURE_2D,old_binded_tex) );
}
void TextureResource::bind_methods(){
    REGISTER_LUA_CONSTANT( TextureWrapMode , REPEAT , REPEAT );
    REGISTER_LUA_CONSTANT( TextureWrapMode , CLAMP , CLAMP );
    REGISTER_LUA_CONSTANT( TextureWrapMode , MIRRORED_REPEAT , MIRRORED_REPEAT );
    REGISTER_LUA_CONSTANT( TextureWrapMode , ALPHA_ZERO , ALPHA_ZERO );

    REGISTER_LUA_MEMBER_FUNCTION( TextureResource , get_size );
    REGISTER_LUA_MEMBER_FUNCTION( TextureResource , get_pixel );
    REGISTER_LUA_MEMBER_FUNCTION( TextureResource , get_wrap_mode );
    REGISTER_LUA_MEMBER_FUNCTION( TextureResource , set_wrap_mode );
}