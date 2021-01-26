#include "resources.h"
#include "resources/lua_script.h"
#include "core.h"


LuaScriptResource::LuaScriptResource( std::string filepath ) : Resource( filepath ){
    src = read_file_as_str(filepath.c_str());
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
    src = read_file_as_str( get_path() );
    return true;
}

void LuaScriptResource::bind_methods(){
    
}