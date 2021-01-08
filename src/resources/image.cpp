#include "resources/image.h"
#include "core.h"

#include <png.h>
#include <string.h>
#include "debug.h"


ImageResource::ImageResource( std::string filepath ) : Resource(filepath) {

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
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_NEAREST ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_NEAREST ) );
            GL_CALL( glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data) );
            GL_CALL( glBindTexture(GL_TEXTURE_2D,old_binded_tex) );
            return;
        }
    }
    else saucer_err( "Error loading image from " , filepath )    
}
ImageResource::~ImageResource(){
    delete[] data;
    GL_CALL( glDeleteTextures(1,&tex_id) );
}
Color           ImageResource::get_pixel(size_t x , size_t y) const{
    size_t r_pointer = 4*( x + y*width );
    return Color( data[r_pointer] , data[r_pointer+1] , data[r_pointer+2] , data[r_pointer+3] );
}
Vector2           ImageResource::get_size() const{
    return Vector2(width,height);
}
void ImageResource::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( ImageResource , get_size );
    REGISTER_LUA_MEMBER_FUNCTION( ImageResource , get_pixel );
}