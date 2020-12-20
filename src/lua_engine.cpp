#include "lua_engine.h"

#include <string.h>

#include <iostream>

size_t LuaEngine::chunk_reader_offset = 0;

LuaEngine::LuaEngine(){
    std::cout << "Creating Lua state..." << std::endl;
    ls = lua_open();
    std::cout << "Loading lua libs..." << std::endl;
    luaopen_base(ls);
    luaopen_io(ls);
    luaopen_string(ls);
    luaopen_math(ls);
    luaopen_table(ls);
    lua_settop(ls,0); // idk why but previous luaopen_[lib] changes the stack so I clean it
    kb_memory_threshold = lua_getgcthreshold(ls);
    kb_memory_used = lua_getgccount(ls);
    std::cout << "Creating lua enviroment..." << std::endl;
    create_env();
    std::cout << "Done." << std::endl;
}
LuaEngine::~LuaEngine(){
    lua_close(ls);
}
void    LuaEngine::create_env( ){
    describe_stack();
    
    // lua_pushvalue(ls,LUA_GLOBALSINDEX);
    
    // Creating engine main table, it will be always at _G["_SAUCER"]
    lua_pushstring(ls,"_SAUCER");
    lua_newtable(ls);
    
    // Creating nodes table
    lua_pushstring(ls,"_NODES");
    lua_newtable(ls);
    lua_settable(ls,2);

    describe_stack();

    lua_settable(ls,LUA_GLOBALSINDEX);

    describe_stack();
}
void    LuaEngine::update_env( SceneNode* new_actor ){

}
void    LuaEngine::describe_stack() const{
    std::cout << "Stack (size=" << lua_gettop(ls) << ")" << std::endl;
    char s[128];
    for( int i = 1 ; i <= lua_gettop(ls) ; i++ ){
        if( ls,lua_type(ls,i) == LUA_TNUMBER ) 
            sprintf(s,"[%d] - [%s] - %f",i,lua_typename(ls,lua_type(ls,i)),lua_tonumber(ls,i));
        else 
            sprintf(s,"[%d] - [%s] - %s",i,lua_typename(ls,lua_type(ls,i)),lua_tostring(ls,i));
        std::cout << s << std::endl;
    }
}
int saucer_print(lua_State* ls){
    std::cout<< lua_tostring( ls , -1 ) << std::endl;
    lua_pop(ls,1);
    return 0;
}

void    LuaEngine::execute( LuaScriptResource* script ){
    
    lua_pushstring(ls, "printa");
    lua_pushcfunction(ls,saucer_print);
    lua_settable(ls,LUA_GLOBALSINDEX);

    chunk_reader_offset = 0;
    int err = lua_load( ls , chunk_reader , (void*)(script->get_src().c_str()) , "." ) || lua_pcall(ls,0,0,0) ;
    
    if( err ){
        std::cerr << "Lua error on " << script->get_path() << std::endl;
        const char* error_msg = "[LUA ERROR]";
        switch (err){
            case LUA_ERRSYNTAX:
                error_msg = "Lua syntax error during pre-compilation"; break;
            case LUA_ERRMEM:
                error_msg = "Lua memory allocation error on "; break;
            case LUA_ERRRUN:
                error_msg = "Lua runtime error "; break;
            case LUA_ERRERR:
                error_msg = "Lua error handling error "; break;
            }
        std::cerr << error_msg << " : " << lua_tostring(ls,-1) << std::endl;
        lua_pop(ls,1); 
    }
}
const char* LuaEngine::chunk_reader( lua_State* p_ls , void* data , size_t* size ){
    size_t total_size = strlen((char*)data);
    if( chunk_reader_offset < total_size ){
        size_t remaining_size = total_size - chunk_reader_offset;
        *size = ( remaining_size < CHUNK_READER_SIZE )?(remaining_size):(CHUNK_READER_SIZE) ;
        char* ret = (char*)(data+chunk_reader_offset);
        chunk_reader_offset += CHUNK_READER_SIZE;
        return ret;
    } else return NULL;
}
