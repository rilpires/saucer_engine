#include "resources.h"
#include "resources/lua_script.h"
#include "core.h"


template<> void LuaEngine::push( lua_State* ls , LuaScriptResource* r ){
    lua_pushnumber(ls,666);
}
LUAENGINE_POP_SAUCER_OBJECT(LuaScriptResource*)    

LuaScriptResource::LuaScriptResource( std::string filepath ) : Resource( filepath ){
    src = read_file_as_string(filepath.c_str());
}
LuaScriptResource::~LuaScriptResource(){
    
}

LuaScriptResource*   LuaScriptResource::get_resource( std::string filepath ){
    return (LuaScriptResource*)ResourceManager::get_resource(filepath);
}

void LuaScriptResource::bind_methods(){
    
}