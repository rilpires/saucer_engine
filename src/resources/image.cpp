#include "resources/image.h"
#include "core.h"

#include <png.h>
#include <string.h>
#include <iostream>


template<> void LuaEngine::push( lua_State* ls , ImageResource* r ){
    if( r ) *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = r->get_saucer_id();
    else    *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = 0;

    lua_newtable(ls);
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls,[](lua_State* ls){
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
        lua_pushcfunction( ls , nested_functions_db["Resource"][arg] ); // Uhhh gotta work around this issue (member inheritance)
        return 1;
    });
    lua_settable(ls,-3);
    lua_setmetatable(ls,-2);
}
LUAENGINE_POP_SAUCER_OBJECT(ImageResource*)   

ImageResource::ImageResource( std::string filepath ) : Resource(filepath) {

    png_image image; /* The control structure used by libpng */

    /* Initialize the 'png_image' structure. */
    memset(&image, 0, (sizeof image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_file(&image,filepath.c_str()) != 0)
    {
        image.format = PNG_FORMAT_RGBA;
        std::cout << "image size: " << PNG_IMAGE_SIZE(image) << std::endl;
        data = new unsigned char[PNG_IMAGE_SIZE(image)];
        width = image.width;
        height = image.height;

        if (data == NULL || png_image_finish_read(&image, NULL/*background*/, data, 0/*row_stride*/, NULL/*colormap*/) == 0){
            png_image_free(&image);
            std::cerr << "Error loading image from " << filepath << std::endl;
        }
        else{
            GL_CALL( glGenTextures(1,&tex_id) );
            GL_CALL( glBindTexture(GL_TEXTURE_2D,tex_id) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_NEAREST ) );
            GL_CALL( glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_NEAREST ) );
            GL_CALL( glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data) );
            return;
        }
    }
    else std::cerr << "Error loading image from " << filepath << std::endl;    
}
ImageResource::~ImageResource(){
    delete[] data;
    GL_CALL( glDeleteTextures(1,&tex_id) );
}
Color           ImageResource::get_pixel(size_t x , size_t y) const{
    size_t r_pointer = 4*( x + y*width );
    return Color( data[r_pointer] , data[r_pointer+1] , data[r_pointer+2] , data[r_pointer+3] );
}
ImageResource*   ImageResource::get_resource(std::string p_resource_path){
    return (ImageResource*)ResourceManager::get_resource(p_resource_path);
}


void ImageResource::bind_methods(){
    
}