#include "lua_engine.h"
#include "core.h"
#include <string.h>
#include "debug.h"
#include <set>


lua_State*  LuaEngine::ls = NULL;
int         LuaEngine::kb_memory_used = 0;
int         LuaEngine::kb_memory_threshold = 0;
size_t      LuaEngine::chunk_reader_offset = 0;
SceneNode*  LuaEngine::current_actor = NULL;

std::unordered_map< std::string , std::unordered_map< std::string , lua_CFunction > > LuaEngine::nested_functions_db;
std::unordered_map< std::string , std::unordered_map< std::string , int > > LuaEngine::constants;
std::unordered_map< std::string , lua_CFunction > LuaEngine::global_functions_db;



template<>
lua_CFunction    LuaEngine::recover_nested_function<SaucerObject>( std::string function_name ){
    auto ret = recover_nested_function( "SaucerObject" , function_name );
    if(!ret){
        saucer_err( "Couldn't find function \"",function_name,"\" for class SaucerObject." )
        return [](lua_State* ls ){return 0;};
    }
    else return ret;
}

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
template<> void     LuaEngine::push( lua_State* ls , SaucerId n ){
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
    saucer_print( "Creating Lua state..." )
    ls = lua_open();
    saucer_print( "Loading lua libs..." )
    luaopen_base(ls);
    luaopen_io(ls);
    luaopen_string(ls);
    luaopen_math(ls);
    luaopen_table(ls);
    lua_settop(ls,0); // idk why but previous luaopen_[lib] changes the stack so I clean it
    saucer_print( "Loading lua binded methods..." )
    Color::bind_methods();
    Input::bind_methods();
    Scene::bind_methods();
    Engine::bind_methods();
    Camera::bind_methods();
    Sprite::bind_methods();
    Vector2::bind_methods();
    Vector3::bind_methods();
    Resource::bind_methods();
    PatchRect::bind_methods();
    Component::bind_methods();
    SceneNode::bind_methods();
    Transform::bind_methods();
    LabelRect::bind_methods();    
    SaucerObject::bind_methods();
    FontResource::bind_methods();
    RenderObject::bind_methods();
    AudioEmitter::bind_methods();
    AnchoredRect::bind_methods();
    CollisionBody::bind_methods();
    AudioResource::bind_methods();
    ResourceManager::bind_methods();
    TextureResource::bind_methods();
    LuaScriptResource::bind_methods();
    kb_memory_threshold = lua_getgcthreshold(ls);
    kb_memory_used = lua_getgccount(ls);
    saucer_print( "Creating lua enviroment..." )
    create_global_env();
    saucer_print( "Done." )
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

    // Pushing nested functions
    for( auto it1 : nested_functions_db ){
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
    // Pushing global functions
    for( auto global_function : global_functions_db ){
        lua_pushstring( ls , global_function.first.c_str() );
        lua_pushcfunction( ls , global_function.second );
        lua_settable( ls , LUA_GLOBALSINDEX );
    }
    // Pushing constants
    for( auto it1 : constants ){
        const std::string& class_name = it1.first;
        lua_pushstring(ls,class_name.c_str());
        lua_newtable(ls);
        for( auto it2 : it1.second ){
            const std::string& index_name = it2.first;
            lua_pushstring(ls,index_name.c_str());
            lua_pushnumber( ls , it2.second );
            lua_settable(ls,-3);
        }
        lua_settable(ls,LUA_GLOBALSINDEX);
    }

    // Pushing global functions constructors(for every type) and destructors (for SaucerObjects)
    // These types can not have static member functions
    // for example, in lua script: 
    //      my_vec = Vector2(50,0)  
    // Vector2 is a function that returns a Vector2 userdata, it can't be
    // used to index any static functions, like "new", used by
    // "by-reference" objects like SceneNode ( my_new_node = SceneNode.new() )
    #define LUAENGINE_VALUE_CONSTRUCTOR(T)                              \
    lua_pushstring( ls , #T );                                          \
    lua_pushcfunction( ls , LuaEngine::create_lua_constructor<T>() );   \
    lua_settable(ls,LUA_GLOBALSINDEX);                                          

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
            lua_settable(ls,-5); // [_G][_SAUCER][_NODES][_ID][key] = [_G][_key]
            // Erasing it from LUA_GLOBALSINDEX
            lua_pushvalue(ls,-2);
            lua_pushnil(ls);
            lua_settable(ls,LUA_GLOBALSINDEX); // [_G][_key] = nil
            lua_pop(ls,1);
        }
        
        // Stack now:
        // 1 - [_SAUCER]
        // 2 - [_SAUCER][_NODES]
        // 3 - [_SAUCER][_NODES][ID]
        // Saving _LOADED variables into virtual environment too, and then cleaning _G[_LOADED]
        lua_pushstring(ls,"_LOADED");
        lua_pushstring(ls,"_LOADED");
        lua_gettable(ls,LUA_GLOBALSINDEX);
        lua_settable(ls,-3);
        lua_pushstring(ls,"_LOADED");
        lua_newtable(ls);
        lua_settable(ls,LUA_GLOBALSINDEX);
        
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
        
        // Stack now:
        // 1 - [_SAUCER]
        // 2 - [_SAUCER][_NODES]
        // 3 - [_SAUCER][_NODES][ID]
        // Loading virtual _LOADED table into the real _LOADED table
        lua_pushstring(ls,"_LOADED");
        lua_pushstring(ls,"_LOADED");
        lua_gettable(ls,-3);
        lua_settable(ls,LUA_GLOBALSINDEX);

        lua_pop(ls,3);
    }
}
void            LuaEngine::describe_stack(){
    saucer_print( "Stack (size=" , lua_gettop(ls) , ")" )
    char s[128];
    for( int i = 1 ; i <= lua_gettop(ls) ; i++ ){
        if( lua_type(ls,i) == LUA_TNUMBER ) 
            sprintf(s,"[%d] - [%s] - %f",i,lua_typename(ls,lua_type(ls,i)),lua_tonumber(ls,i));
        else 
            sprintf(s,"[%d] - [%s] - %s",i,lua_typename(ls,lua_type(ls,i)),lua_tostring(ls,i));
        saucer_print( s )
    }
}
void            LuaEngine::print_error( int err , LuaScriptResource* script ){   
    if( err ){
        saucer_err( "Lua error on " , script->get_path() )
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
        saucer_err( error_msg , " : " , lua_tostring(ls,-1) )
        lua_pop(ls,1);  
        exit(1);
    }
}
lua_CFunction   LuaEngine::recover_nested_function( std::string class_name , std::string function_name ){
    auto class_find = nested_functions_db.find( class_name );
    lua_CFunction ret = nullptr;
    if( class_find != nested_functions_db.end() ){
        auto function_find = class_find->second.find( function_name );
        if( function_find != class_find->second.end() ){
            ret = (function_find->second);
        }
    }
    return ret;
}
lua_CFunction   LuaEngine::recover_global_function( std::string function_name ){
    auto function_find = global_functions_db.find( function_name );
    if( function_find == global_functions_db.end() ){
        saucer_err( "Couldn't find any global function named " , function_name )
        // exit(1);
        return [](lua_State* ls ){ 
            saucer_err( "This function should not exist!" );
            return 0; 
        };
    } else return (function_find->second);
}
void            LuaEngine::execute_collision_start( SceneNode* actor , SceneNode* other ){
    if( actor->get_script() == NULL || actor->get_script()->has_collision_start == false )return;
    SceneNode* old_actor = current_actor;
    change_current_actor_env( actor );
    lua_pushstring(ls,"_collision_start");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    push(ls,other);
    int err = lua_pcall(ls,1,0,0);
    print_error(err,actor->get_script());
    change_current_actor_env(old_actor);
}
void            LuaEngine::execute_collision_end( SceneNode* actor , SceneNode* other ){
    if( actor->get_script() == NULL || actor->get_script()->has_collision_end == false )return;
    SceneNode* old_actor = current_actor;
    change_current_actor_env( actor );
    lua_pushstring(ls,"_collision_end");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    push(ls,other);
    int err = lua_pcall(ls,1,0,0);
    print_error(err,actor->get_script());
    change_current_actor_env(old_actor);
}
void            LuaEngine::execute_frame_start( SceneNode* actor , float delta_seconds ){
    if( actor->get_script() == NULL || actor->get_script()->has_frame_start == false )return;
    SceneNode* old_actor = current_actor;
    change_current_actor_env( actor );
    lua_pushstring(ls,"_frame_start");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    lua_pushnumber(ls,delta_seconds);
    int err = lua_pcall(ls,1,0,0);
    print_error(err,actor->get_script());
    change_current_actor_env(old_actor);
}
void            LuaEngine::execute_input( SceneNode* actor , Input::InputEvent* input_event ){
    if( actor->get_script() == NULL || actor->get_script()->has_input == false )return;
    SceneNode* old_actor = current_actor;
    change_current_actor_env( actor );
    lua_pushstring(ls,"_input");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    push(ls,input_event);
    int err = lua_pcall(ls,1,0,0);
    print_error(err,actor->get_script());
    change_current_actor_env(old_actor);
}
void            LuaEngine::execute_entered_tree( SceneNode* actor ){
    if( actor->get_script() == NULL || actor->get_script()->has_entered_tree == false )return;
    SceneNode* old_actor = current_actor;
    change_current_actor_env( actor );
    lua_pushstring(ls,"_entered_tree");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    int err = lua_pcall(ls,0,0,0);
    print_error(err,actor->get_script());
    change_current_actor_env(old_actor);
}
void            LuaEngine::execute_exiting_tree( SceneNode* actor ){
    if( actor->get_script() == NULL || actor->get_script()->has_exiting_tree == false )return;
    SceneNode* old_actor = current_actor;
    change_current_actor_env( actor );
    lua_pushstring(ls,"_exiting_tree");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    int err = lua_pcall(ls,0,0,0);
    print_error(err,actor->get_script());
    change_current_actor_env(old_actor);
}
void            LuaEngine::execute_init( SceneNode* actor ){
    if( actor->get_script() == NULL || actor->get_script()->has_init == false )return;
    SceneNode* old_actor = current_actor;
    change_current_actor_env( actor );
    lua_pushstring(ls,"_init");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    int err = lua_pcall(ls,0,0,0);
    print_error(err,actor->get_script());
    change_current_actor_env(old_actor);
}
void            LuaEngine::create_actor_env( SceneNode* new_actor ){
    SceneNode* old_actor = current_actor;
    change_current_actor_env(NULL);
    std::set<std::string> old_names;

    lua_pushnil(ls);
    while( lua_next(ls,LUA_GLOBALSINDEX)!=0 ){
        if( !lua_isstring(ls,-2) ){
            saucer_err( "Oops? table key isn't string, it is: " , lua_typename(ls,lua_type(ls,-2)) , "\t"
            , "converted to string: " , lua_tostring(ls,-2) );
        } else {
            old_names.insert( lua_tostring(ls,-2) );
        }
        lua_pop(ls,1);
    }

    chunk_reader_offset = 0;
    LuaScriptResource* script = new_actor->get_script();
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
            saucer_err( "Oops? table key isn't string, it is: " , lua_typename(ls,lua_type(ls,-2)) , "\t" , "converted to string: " , lua_tostring(ls,-2) );
        } else if( old_names.find( lua_tostring(ls,-2) ) == old_names.end() ) {
            
            std::string key = lua_tostring(ls,-2);

            // Saves in this script resource if it has any definition of functions "_frame_start","_input", etc
            // So we don't need to access it's actor's table every time for it 
            if( key == "_frame_start" )          script->has_frame_start = true;
            else if( key == "_input" )           script->has_input = true;
            else if( key == "_entered_tree" )    script->has_entered_tree = true;
            else if( key == "_exiting_tree" )     script->has_exiting_tree = true;
            else if( key == "_collision_start" ) script->has_collision_start = true;
            else if( key == "_collision_end" )   script->has_collision_end = true;
            else if( key == "_init" )            script->has_init = true;
            

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
    push( ls , new_actor );
    lua_settable(ls,-3);
    // Creating a virtual "_LOADED" table to save loaded modules only for this actor
    lua_pushstring(ls,"_LOADED");
    lua_newtable(ls);
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
void    LuaEngine::register_constant( std::string enum_name , std::string index_name , int i ){
    constants[enum_name][index_name] = i;
}
void    LuaEngine::register_function( std::string global_function_name , lua_CFunction f ){
    global_functions_db[global_function_name] = f;
}
void    LuaEngine::register_function( std::string class_name , std::string function_name , lua_CFunction f ){
    nested_functions_db[class_name][function_name] = f;
}
