#include "resources.h"
#include "resources/image.h"
#include "resources/lua_script.h"
#include "resources/audiofile.h"
#include "resources/node_template.h"
#include "resources/shader.h"
#include "resources/font.h"
#include "lua_engine.h"

#include "debug.h"
#include <fstream>
#include <sstream>

std::unordered_map<std::string,SaucerId> ResourceManager::id_by_path;

Resource::Resource(){
    dirty = false;
}
Resource::Resource( std::string filepath ){
    path = filepath;
    dirty = false;
}
Resource::~Resource(){
}
std::string         Resource::get_path() const {
    return path;
}
void                Resource::flag_as_dirty(){
    ;
}
bool        Resource::reload(){
    return true;
}

std::string Resource::read_file_as_str( std::string filename ){
    std::ifstream ifs( filename , std::ifstream::in );
    if( !ifs ){
        saucer_err( "Couldn't open file " , filename )
    } else {
        std::ostringstream sstr;
        sstr << ifs.rdbuf();
        return sstr.str();
    }
    return "";
}
void Resource::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION(Resource,get_path);
}


template<> 
Resource*           ResourceManager::get_resource( std::string p_resource_path){
    auto it = id_by_path.find(p_resource_path);
    if( it == id_by_path.end() )
        return load_resource(p_resource_path);
    else{
        Resource* ret = static_cast<Resource*>(SaucerObject::from_saucer_id(it->second));
        if( ret->dirty ){
            if(ret->reload()==false){
                saucer_err("Failed to reload resource: " , p_resource_path );
            } else {
                ret->dirty = false;
            }
        }
        return ret;
    }
}

void    ResourceManager::set_resource(std::string resource_name , Resource* r ){
    if( r == NULL )return;
    if( id_by_path.find(resource_name)!= id_by_path.end()){
        if( id_by_path[resource_name]!= r->get_saucer_id() ){
            saucer_err( "Resource name " , resource_name , " is already used by another resource" );
            return;
        } else return; // Nothing to do
    }
    r->path = resource_name;
    id_by_path[resource_name] = r->get_saucer_id();
}
Resource*           ResourceManager::load_resource(std::string filepath){
    std::string str_filepath = filepath;
    Resource* ret = NULL;
    size_t pos = str_filepath.find_last_of('.');
    std::string extension = "";
    if( pos != std::string::npos ) extension = str_filepath.substr( pos );
    
    try
    {
        if( extension == ".png" ){
            ret = new TextureResource( filepath );
        }
        else if (extension == ".wav"){
            ret = new WavAudioResource( filepath.c_str() );
        }
        else if (extension == ".lua"){
            ret = new LuaScriptResource( filepath );
        }
        else if (extension == ".ttf"){
            ret = new FontResource( filepath );
        }
        else if (extension == ".node"){
            ret = new NodeTemplateResource( filepath );
        }
        else if (extension == ".glsl"){
            ret = new ShaderResource( filepath );
        } else {
            if(str_filepath.size()>0)saucer_err( "What is this? Couldn't load resource for: " , filepath );
        }
    }
    catch(const std::exception& e)
    {
        ret = nullptr;
        saucer_err("Error loading resource " , filepath , ": " , e.what() );
    }
    if(ret) id_by_path.insert(std::pair<std::string,SaucerId>(ret->path ,ret->get_saucer_id()));
    return ret;
}
void    ResourceManager::free_resource(Resource* p_resource){
    id_by_path.erase( p_resource->get_path());
    delete p_resource;
}
void    ResourceManager::dirty_every_resource(){
    for( auto& it : id_by_path ){
        Resource* res = static_cast<Resource*>( SaucerObject::from_saucer_id(it.second) );
        res->flag_as_dirty();
    }
}
const decltype(ResourceManager::id_by_path)::iterator ResourceManager::begin(){
    return id_by_path.begin();
}
const decltype(ResourceManager::id_by_path)::iterator ResourceManager::end()  {
    return id_by_path.end();
}   
void    ResourceManager::bind_methods(){
    REGISTER_LUA_GLOBAL_FUNCTION( "load" , ResourceManager::get_resource<Resource> );
}
