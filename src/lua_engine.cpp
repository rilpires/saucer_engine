#include "lua_engine.h"
#include "core.h"
#include <string.h>
#include <iostream>
#include <set>

lua_State*  LuaEngine::ls = NULL;
int         LuaEngine::kb_memory_used = 0;
int         LuaEngine::kb_memory_threshold = 0;
size_t      LuaEngine::chunk_reader_offset = 0;
SceneNode*  LuaEngine::current_actor = NULL;

std::unordered_map< std::string , std::unordered_map< std::string , lua_CFunction > > LuaEngine::c_function_db;


template<> void     LuaEngine::push( lua_State* ls , bool b ){
    lua_pushboolean(ls,b);
}
template<> void     LuaEngine::push( lua_State* ls , std::string s ){
    lua_pushstring(ls,s.c_str());
}      
template<> void     LuaEngine::push( lua_State* ls , double n ){
    lua_pushnumber(ls,n);
}   
template<> void     LuaEngine::push( lua_State* ls , float n ){
    lua_pushnumber(ls,n);
}   
template<> void     LuaEngine::push( lua_State* ls , int n ){
    lua_pushnumber(ls,n);
}   
template<> void     LuaEngine::push( lua_State* ls , short n ){
    lua_pushnumber(ls,n);
}   

template<> int          LuaEngine::pop( lua_State* ls ){
    int ret = lua_tonumber(ls,-1);
    lua_pop(ls,1);
    return ret;
} 
template<> float        LuaEngine::pop( lua_State* ls ){
    float ret = lua_tonumber(ls,-1);
    lua_pop(ls,1);
    return ret;
} 
template<> short        LuaEngine::pop( lua_State* ls ){
    short ret = lua_tonumber(ls,-1);
    lua_pop(ls,1);
    return ret;
} 
template<> double       LuaEngine::pop( lua_State* ls ){
    double ret = lua_tonumber(ls,-1);
    lua_pop(ls,1);
    return ret;
} 
template<> bool         LuaEngine::pop( lua_State* ls ){ 
    bool ret = lua_toboolean(ls,-1);
    lua_pop(ls,1);
    return ret;
} 
template<> std::string  LuaEngine::pop( lua_State* ls ){ 
    std::string ret = lua_tostring(ls,-1);
    lua_pop(ls,1);
    return ret; 
} 


const char*     LuaEngine::chunk_reader( lua_State* ls , void* data , size_t* size ){
    size_t total_size = strlen((char*)data);
    if( chunk_reader_offset < total_size ){
        size_t remaining_size = total_size - chunk_reader_offset;
        *size = ( remaining_size < CHUNK_READER_SIZE )?(remaining_size):(CHUNK_READER_SIZE) ;
        char* ret = &((char*)data)[chunk_reader_offset];
        chunk_reader_offset += CHUNK_READER_SIZE;
        return ret;
    } else return NULL;
}
void            LuaEngine::initialize(){
    std::cout << "Creating Lua state..." << std::endl;
    ls = lua_open();
    std::cout << "Loading lua libs..." << std::endl;
    luaopen_base(ls);
    luaopen_io(ls);
    luaopen_string(ls);
    luaopen_math(ls);
    luaopen_table(ls);
    lua_settop(ls,0); // idk why but previous luaopen_[lib] changes the stack so I clean it
    std::cout << "Loading lua binded methods..." << std::endl;
    Engine::bind_methods();
    Scene::bind_methods();
    SceneNode::bind_methods();
    Vector2::bind_methods();
    Vector3::bind_methods();
    Transform::bind_methods();
    Color::bind_methods();
    Resource::bind_methods();
    ImageResource::bind_methods();
    LuaScriptResource::bind_methods();
    ResourceManager::bind_methods();
    kb_memory_threshold = lua_getgcthreshold(ls);
    kb_memory_used = lua_getgccount(ls);
    std::cout << "Creating lua enviroment..." << std::endl;
    create_global_env();
    std::cout << "Done." << std::endl;
}
void            LuaEngine::finish(){
    lua_close(ls);
}
void            LuaEngine::create_global_env( ){
    // Creating engine main table, it will be always at _G["_SAUCER"]
    lua_pushstring(ls,"_SAUCER");
    lua_newtable(ls);
    // Creating nodes table
    lua_pushstring(ls,"_NODES");
    lua_newtable(ls);
    lua_settable(ls,2);
    lua_settable(ls,LUA_GLOBALSINDEX);

    // Pushing global functions: constructors(for every type) and destructors (for SaucerObjects)
    #define LUAENGINE_VALUE_CONSTRUCTOR(T)                              \
    lua_pushstring( ls , #T );                                          \
    lua_pushcfunction( ls , LuaEngine::create_lua_constructor<T>(ls) ); \
    lua_settable(ls,LUA_GLOBALSINDEX);                                          


    for( auto it1 : c_function_db ){
        const std::string& class_name = it1.first;
        lua_pushstring(ls,class_name.c_str());
        lua_newtable(ls);
        for( auto it2 : it1.second ){
            const std::string& function_name = it2.first;
            if( function_name == "lua_new" ){
                lua_pushstring( ls , "new" );
            }
            else{
                lua_pushstring(ls,function_name.c_str());
            }
            lua_pushcfunction( ls , it2.second );
            lua_settable(ls,-3);
        }
        lua_settable(ls,LUA_GLOBALSINDEX);
    }

    LUAENGINE_VALUE_CONSTRUCTOR(Color);
    LUAENGINE_VALUE_CONSTRUCTOR(Vector2);
    LUAENGINE_VALUE_CONSTRUCTOR(Vector3);

}
void            LuaEngine::change_current_actor_env( SceneNode* new_actor ){
    if( current_actor == new_actor ) return;
    if( current_actor ){
        lua_pushfstring(ls,"_SAUCER");
        lua_gettable(ls,LUA_GLOBALSINDEX);
        lua_pushfstring(ls,"_NODES");
        lua_gettable(ls,-2);
        lua_pushnumber(ls,current_actor->get_saucer_id());
        lua_gettable(ls,-2);
        lua_pushnil(ls);
        while( lua_next(ls,-2)!=0 ){
            // Saving it into it's context table
            lua_pushvalue(ls,-2);
            lua_pushvalue(ls,-3);
            lua_gettable(ls,LUA_GLOBALSINDEX);
            lua_settable(ls,-5);
            // Erasing it from LUA_GLOBALSINDEX
            lua_pushvalue(ls,-2);
            lua_pushnil(ls);
            lua_settable(ls,LUA_GLOBALSINDEX);
            lua_pop(ls,1);
        }
        lua_pop(ls,3);
    }
    current_actor = new_actor;
    if( current_actor ){
        lua_pushfstring(ls,"_SAUCER");
        lua_gettable(ls,LUA_GLOBALSINDEX);
        lua_pushfstring(ls,"_NODES");
        lua_gettable(ls,-2);
        lua_pushnumber(ls,current_actor->get_saucer_id());
        lua_gettable(ls,-2);
        lua_pushnil(ls);
        while( lua_next(ls,-2)!=0 ){
            lua_pushvalue(ls,-2);
            lua_pushvalue(ls,-2);
            lua_settable(ls,LUA_GLOBALSINDEX);
            lua_pop(ls,1);
        }
        lua_pop(ls,3);
    }
}
void            LuaEngine::describe_stack(){
    std::cout << "Stack (size=" << lua_gettop(ls) << ")" << std::endl;
    char s[128];
    for( int i = 1 ; i <= lua_gettop(ls) ; i++ ){
        if( lua_type(ls,i) == LUA_TNUMBER ) 
            sprintf(s,"[%d] - [%s] - %f",i,lua_typename(ls,lua_type(ls,i)),lua_tonumber(ls,i));
        else 
            sprintf(s,"[%d] - [%s] - %s",i,lua_typename(ls,lua_type(ls,i)),lua_tostring(ls,i));
        std::cout << s << std::endl;
    }
}
void            LuaEngine::print_error( int err , LuaScriptResource* script ){   
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
        exit(1);
    }
}
lua_CFunction   LuaEngine::recover_cfunction( std::string class_name , std::string function_name ){
    auto class_find = c_function_db.find( class_name );
    if( class_find == c_function_db.end() ){
        std::cerr << "Couldn't find any function for class " << class_name << std::endl;
        // exit(1);
    } else {
        auto function_find = class_find->second.find( function_name );
        if( function_find == class_find->second.end() ){
            std::cerr << "Couldn't find any function named " << function_name << " for class " << class_name << std::endl;
            // exit(1);
        } else {
            return function_find->second;
        }
    }
    return [](lua_State* ls ){ 
        // std::cerr << "This function should not be called!" << std::endl ; 
        return 0; 
    };
}
void            LuaEngine::execute_frame_start( SceneNode* actor ){
    change_current_actor_env( actor );
    lua_pushstring(ls,"frame_start");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    lua_pushnumber(ls,0.15);
    int err = lua_pcall(ls,1,0,0);

    print_error(err,actor->get_script_resource());
}
void            LuaEngine::execute_input_event( SceneNode* actor ){

}
void            LuaEngine::create_actor_env( SceneNode* new_actor ){
    SceneNode* old_actor = current_actor;
    change_current_actor_env(NULL);
    std::set<std::string> old_names;

    lua_pushnil(ls);
    while( lua_next(ls,LUA_GLOBALSINDEX)!=0 ){
        if( !lua_isstring(ls,-2) ){
            std::cerr << "Oops? table key isn't string, it is: " << lua_typename(ls,lua_type(ls,-2)) << "\t"
            << "converted to string: " << lua_tostring(ls,-2) << std::endl;
        } else {
            old_names.insert( lua_tostring(ls,-2) );
        }
        lua_pop(ls,1);
    }

    chunk_reader_offset = 0;
    LuaScriptResource* script = new_actor->get_script_resource();
    int err = lua_load( ls , chunk_reader , (void*)(script->get_src().c_str()) , "." ) || lua_pcall(ls,0,0,0) ;
    print_error(err,script);

    // Creating this actor env table
    lua_newtable(ls);

    lua_pushstring(ls,"_SAUCER_ID");
    lua_pushnumber(ls,new_actor->get_saucer_id());
    lua_settable(ls,1);

    lua_pushnil(ls);
    while( lua_next(ls,LUA_GLOBALSINDEX)!=0 ){
        if( !lua_isstring(ls,-2) ){
            std::cerr << "Oops? table key isn't string, it is: " << lua_typename(ls,lua_type(ls,-2)) << "\t"
            << "converted to string: " << lua_tostring(ls,-2) << std::endl;
        } else if( old_names.find( lua_tostring(ls,-2) ) == old_names.end() ) {
            lua_pushvalue(ls,-2);
            lua_pushvalue(ls,-2);
            lua_settable(ls,-5);
            // Erasing from GLOBALSINDEX!
            lua_pushvalue(ls,-2);
            lua_pushnil(ls);
            lua_settable(ls,LUA_GLOBALSINDEX);
        }
        lua_pop(ls,1);
    }

    // Creating the userdata (that holds only the node id) and it's metatable
    lua_pushstring(ls,"this");
    //push( ls , std::forward<SceneNode*&&>(new_actor) );
    push( ls , new_actor );
    lua_settable(ls,-3);

    // Now inserting it into _G["_SAUCER"]["_NODES"][id]
    lua_pushstring(ls,"_SAUCER");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    lua_pushstring(ls,"_NODES");
    lua_gettable(ls,-2);
    lua_pushnumber(ls,new_actor->get_saucer_id());
    lua_pushvalue(ls,1);
    lua_settable(ls,-3);
    lua_pop(ls,3);

    change_current_actor_env(old_actor);
}
void            LuaEngine::register_function( std::string class_name , std::string function_name , lua_CFunction f ){
    c_function_db[class_name][function_name] = f;
}
