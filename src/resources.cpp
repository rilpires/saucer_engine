#include "resources.h"
#include "resources/image.h"
#include "resources/lua_script.h"

#include <iostream>

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
    std::string extension = str_filepath.substr( str_filepath.find_last_of('.') );

    if( extension == ".png" ){
        ret = new ImageResource( filepath );
    }
    else if (extension == ".lua"){
        ret = new LuaScriptResource( filepath );
    } else {
        std::cerr << "Couldn't load resource for filepath: " << filepath << std::endl;
    }
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

