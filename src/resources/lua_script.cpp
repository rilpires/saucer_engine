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
bool    LuaScriptResource::reload(){
    //
    return true;
}

void LuaScriptResource::bind_methods(){
    
}