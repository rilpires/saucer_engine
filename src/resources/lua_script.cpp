#include "resources.h"
#include "resources/lua_script.h"
#include "core.h"


LuaScriptResource::LuaScriptResource( std::string filepath ) : Resource( filepath ){
    src = read_file_as_str(filepath.c_str());
}
LuaScriptResource::~LuaScriptResource(){
    
}

LuaScriptResource*   LuaScriptResource::get_resource( std::string filepath ){
    return (LuaScriptResource*)ResourceManager::get_resource(filepath);
}

void LuaScriptResource::bind_methods(){
    
}