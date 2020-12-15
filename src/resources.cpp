#include "resources.h"

#include <iostream>
#include <string.h>
#include <png.h>
#include "core.h"

ResourceId  Resource::open_resource_id = 0;
Resource::Resource(){
    resource_id = Resource::open_resource_id++;
}
Resource::Resource( std::string filepath ){
    resource_id = Resource::open_resource_id++;
    path = filepath;
}
Resource::~Resource(){
}

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

NumberResource*  NumberResource::get_resource(std::string p_resource_path){
    return (NumberResource*)ResourceManager::get_resource(p_resource_path);
}


ResourceManager*    ResourceManager::singleton = NULL;
ResourceManager::ResourceManager(){

}
ResourceManager::~ResourceManager(){
    for( auto it = resources.begin() ; it != resources.end() ; it++ ){
        std::cout << "Removing resource: " << it->second->path << std::endl;
        delete it->second;
    }
}
ResourceManager*    ResourceManager::instance(){
    if(!singleton)singleton = new ResourceManager();
    return singleton;
}
Resource*           ResourceManager::get_resource(ResourceId p_id){
    return instance()->resources[p_id];
}
Resource*           ResourceManager::get_resource( std::string p_resource_path){
    ResourceManager* rm = instance();
    auto it = rm->id_by_path.find(p_resource_path);
    if( it == rm->id_by_path.end() )
        return load_resource(p_resource_path);
    else 
        return get_resource(it->second);
}

void                ResourceManager::set_resource(std::string resource_name , Resource* r ){
    if(r==NULL)return;
    ResourceManager* rm = instance();
    if(rm->id_by_path.find(resource_name)!=rm->id_by_path.end()){
        if( rm->id_by_path[resource_name]!=r->resource_id ){
            std::cerr << "Resource name " << resource_name << " is already used by another resource" << std::endl;
            return;
        }
    }
    r->path = resource_name;
    rm->resources[r->resource_id] = r;
    rm->id_by_path[resource_name] = r->resource_id;
}
Resource*           ResourceManager::load_resource(std::string filepath){
    std::string str_filepath = filepath;
    Resource* ret = NULL;
    if( str_filepath.substr( str_filepath.size()-4 , 4 ) == ".png" ){
        ret = new ImageResource( filepath );
    }
    if(ret==NULL) std::cerr << "Couldn't load resource for filepath=" << filepath << std::endl;
    instance()->resources.insert(std::pair<ResourceId,Resource*>(ret->resource_id,ret));
    instance()->id_by_path.insert(std::pair<std::string,ResourceId>(ret->path ,ret->resource_id));
    return ret;
}
void                ResourceManager::free_resource(Resource* p_resource){
    ResourceManager* rm = instance();
    auto it = rm->resources.find(p_resource->resource_id);
    if( it != rm->resources.end() ){
        Resource* r = it->second;
        rm->resources.erase(it->first);
        rm->id_by_path.erase(r->get_path());
        delete r;
    }
}

