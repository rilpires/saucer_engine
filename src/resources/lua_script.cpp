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
LuaScriptResource*   LuaScriptResource::get_resource( std::string filepath ){
    return (LuaScriptResource*)ResourceManager::get_resource(filepath);
}

void LuaScriptResource::bind_methods(){
    
}