#include "saucer_object.h"
#include "lua_engine.h"

SaucerObject::SaucerId SaucerObject::open_id = 1;
std::unordered_map<SaucerObject::SaucerId,SaucerObject*> SaucerObject::saucer_objs;
uint32_t __open_saucer_class_id = 1;

std::vector<void*>& __class_bind_methods(){
    static std::vector<void*> ret;
    return ret;
}
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

YamlNode    SaucerObject::to_yaml_node() const{
    YamlNode ret;
    ret["SaucerObject"] = get_saucer_id();
    ret["Error"] = "This class probably doesn't has \"to_yaml_node\" implemented.";
    return ret;
}
void    SaucerObject::from_yaml_node(YamlNode n){
    UNUSED(n);
}
void    SaucerObject::from_yaml_node( std::string p  ){
    YamlNode yaml_node;
    try { 
        yaml_node = YAML::LoadFile( p ); 
        from_yaml_node(yaml_node);
    }
    catch(const YAML::BadFile::Exception& e) { 
        saucer_err( "YAML::BadFile: " , p );
    }
}
void    SaucerObject::save_as_file( std::string p ) const {
    std::ofstream ofs;
    ofs.open( p , std::ofstream::out );
    ofs << this->to_yaml_node() << '\n';
    ofs.close();
}
void    SaucerObject::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( SaucerObject , get_saucer_id )   
}