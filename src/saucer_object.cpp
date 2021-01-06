#include "saucer_object.h"
#include "lua_engine.h"

SaucerObject::SaucerId SaucerObject::open_id = 1;
std::unordered_map<SaucerObject::SaucerId,SaucerObject*> SaucerObject::saucer_objs;


SaucerObject::SaucerObject(){
    id = open_id++;
    saucer_objs[id] = this;
}
SaucerObject::~SaucerObject(){
    saucer_objs.erase(id);
}
SaucerObject::SaucerId        SaucerObject::get_saucer_id() const{
    return id;
}
SaucerObject*   SaucerObject::from_saucer_id( const SaucerId& p_id ){
    auto it = saucer_objs.find(p_id);
    if( it != saucer_objs.end() ) return it->second;
    else return NULL;
}

void SaucerObject::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( SaucerObject , get_saucer_id )   
}