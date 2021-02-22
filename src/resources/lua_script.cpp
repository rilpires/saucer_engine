#include "resources.h"
#include "resources/lua_script.h"
#include "core.h"


LuaScriptResource::LuaScriptResource( const std::vector<uint8_t>& mem_data ) {
    src.resize( mem_data.size() );
    memcpy( &(src[0]) , &(mem_data[0]) , src.size() );
}
LuaScriptResource::~LuaScriptResource(){
    
}

bool    LuaScriptResource::has_callback( std::string callback_name ) const{
    return existent_callbacks.find(callback_name) != existent_callbacks.end();
}  
void    LuaScriptResource::flag_as_dirty(){
    dirty = true;
}
void    LuaScriptResource::reload(){
    std::ifstream ifs{ path };
    ifs.unsetf(std::ios::skipws);
    std::istream_iterator<char> begin{ifs} , end;
    src = std::string( begin , end );
    dirty = false;
    ifs.close();
}

void LuaScriptResource::bind_methods(){
    
}