#include "resources.h"
#include "resources/image.h"
#include "resources/lua_script.h"
#include "lua_engine.h"

#include <iostream>

std::unordered_map<std::string,SaucerId> ResourceManager::id_by_path;

template<> void LuaEngine::push( lua_State* ls , Resource* r ){
    if( r ) *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = r->get_saucer_id();
    else    *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = 0;

    lua_newtable(ls);
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls,[](lua_State* ls){
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
        lua_pushcfunction( ls , LuaEngine::LuaEngine::recover_cfunction("Resource",arg) );
        return 1;
    });
    lua_settable(ls,-3);
    lua_setmetatable(ls,-2);
}
LUAENGINE_POP_SAUCER_OBJECT(Resource*)   

Resource::Resource(){
}
Resource::Resource( std::string filepath ){
    path = filepath;
}
Resource::~Resource(){
}


void Resource::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION(Resource,get_path);
}

Resource*           ResourceManager::get_resource( std::string p_resource_path){
    auto it = id_by_path.find(p_resource_path);
    if( it == id_by_path.end() )
        return load_resource(p_resource_path);
    else 
        return static_cast<Resource*>(SaucerObject::from_saucer_id(it->second));
}

void    ResourceManager::set_resource(std::string resource_name , Resource* r ){
    if( r == NULL )return;
    if( id_by_path.find(resource_name)!= id_by_path.end()){
        if( id_by_path[resource_name]!= r->get_saucer_id() ){
            std::cerr << "Resource name " << resource_name << " is already used by another resource" << std::endl;
            return;
        } else return; // Nothing to do
    }
    r->path = resource_name;
    id_by_path[resource_name] = r->get_saucer_id();
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
        std::cerr << "What is this? Couldn't load resource for filepath: " << filepath << std::endl;
    }
    id_by_path.insert(std::pair<std::string,SaucerId>(ret->path ,ret->get_saucer_id()));
    return ret;
}
void    ResourceManager::free_resource(Resource* p_resource){
    id_by_path.erase( p_resource->get_path());
    delete p_resource;
}

void    ResourceManager::bind_methods(){
    REGISTER_LUA_STATIC_FUNCTION(ResourceManager,get_resource);    
}
