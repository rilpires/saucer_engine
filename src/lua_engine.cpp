#include "lua_engine.h"
#include "core.h"
#include <string.h>
#include "debug.h"
#include <set>
#include "editor.h"

lua_State*  LuaEngine::ls = NULL;
size_t      LuaEngine::chunk_reader_offset = 0;
SceneNode*  LuaEngine::current_actor = NULL;

std::unordered_map< std::string , std::unordered_map< std::string , lua_CFunction > > LuaEngine::nested_functions_db;
std::unordered_map< std::string , std::unordered_map< std::string , int > > LuaEngine::constants;
std::unordered_map< std::string , lua_CFunction > LuaEngine::global_functions_db;
std::set<std::string> LuaEngine::vanilla_global_keys;

template<>
lua_CFunction    LuaEngine::recover_nested_function<SaucerObject>( std::string function_name ){
    auto ret = recover_nested_function( "SaucerObject" , function_name );
    return ret;
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


template<> void     LuaEngine::push_metatable<SceneNode>( lua_State* ls ){
    lua_newtable(ls);
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls,[](lua_State* ls){
        SceneNode* node = (SceneNode*)SaucerObject::from_saucer_id( *(SaucerId*)lua_touserdata(ls,-2) );
        SAUCER_ASSERT(node , "Trying to access a SceneNode that was previously freed.");
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
        lua_CFunction nested_function = LuaEngine::recover_nested_function<SceneNode>(arg);
        if( nested_function != nullptr ){
            lua_pushcfunction( ls , nested_function );
        } else {
            lua_pushstring(ls,"_SAUCER");
            lua_gettable(ls,LUA_GLOBALSINDEX);
            lua_pushstring(ls,"_NODES");
            lua_gettable(ls,-2);
            lua_pushnumber(ls,node->get_saucer_id());
            lua_gettable(ls,-2);
            lua_pushstring(ls,arg);
            lua_gettable(ls,-2);
            lua_insert(ls,1);
            lua_pop(ls,3);
        }
        return 1;
    });
    lua_settable(ls,-3);
    lua_pushstring(ls,"__newindex");
    lua_pushcfunction(ls,[](lua_State* ls){
        SceneNode* node = (SceneNode*)SaucerObject::from_saucer_id( *(SaucerId*)lua_touserdata(ls,-3) );
        const char* key = lua_tostring(ls,-2);
        lua_pushstring(ls,"_SAUCER");
        lua_gettable(ls,LUA_GLOBALSINDEX);
        lua_pushstring(ls,"_NODES");
        lua_gettable(ls,-2);
        lua_pushnumber(ls,node->get_saucer_id());
        lua_gettable(ls,-2);
        lua_pushstring(ls,key);
        lua_pushvalue(ls,-5);
        lua_settable(ls,-3);
        lua_pop(ls,6);
        return 0;
    });
    lua_settable(ls,-3);
}


const char*     LuaEngine::chunk_reader( lua_State* ls , void* data , size_t* size ){
    UNUSED(ls);
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
    saucer_print( "Loading lua binded methods..." , __class_bind_methods().size() , " on total" );

    for( void* bind_method_pointer : __class_bind_methods() ) 
        (*(void(*)())(bind_method_pointer))(); // Ugh
    
    // These aren't saucer objects so are not included in __class_bind_methods
    Color::bind_methods();
    Vector2::bind_methods();
    Vector3::bind_methods();
    Transform::bind_methods();
    

    // Registering a custom print function
    #ifdef SAUCER_EDITOR
    LuaEngine::register_function("print", [](lua_State* ls){
        SaucerEditor::stream << "[LUA]\t";
        for( int i = 1 ; i <= lua_gettop(ls) ; i++ ){
            if( lua_isnumber(ls,i) )        SaucerEditor::stream << lua_tonumber(ls,i);
            else if (lua_isstring(ls,i))    SaucerEditor::stream << lua_tostring(ls,i);
            else if (lua_isboolean(ls,i))   SaucerEditor::stream << ( ( lua_toboolean(ls,-1) ) ? ("true") : ("false") );
            else                            SaucerEditor::stream << "[" << lua_typename(ls,lua_type(ls,i)) << "]";
        }
        SaucerEditor::stream << std::endl;
        return 0;
    });
    #endif

    saucer_print( "Creating lua enviroment..." )
    create_global_env();
    // Now we fill the global vanilla keys. Anything defined after it will be erased when LuaEngine::reset is called
    vanilla_global_keys.clear();
    lua_pushnil(ls);
    while( lua_next(ls,LUA_GLOBALSINDEX) ){
        lua_pop(ls,1);
        SAUCER_ASSERT( lua_type(ls,-1) == LUA_TSTRING , "Is this even possible?");
        vanilla_global_keys.insert( std::string(lua_tostring(ls,-1)));
    }
    saucer_print( "Done." )
}
void            LuaEngine::reset(){
    lua_pushnil(ls);
    while( lua_next(ls,LUA_GLOBALSINDEX) ){
        lua_pop(ls,1);
        if( vanilla_global_keys.find(lua_tostring(ls,-1)) == vanilla_global_keys.end() ){
            lua_pushvalue(ls,-1);
            lua_pushnil(ls);
            lua_settable(ls,LUA_GLOBALSINDEX);
        }
    }
}
void            LuaEngine::finish(){
    lua_close(ls);
}
int             LuaEngine::get_kb_memory_used(){
    return lua_getgccount(ls);
}
int             LuaEngine::get_kb_memory_threshold(){
    return lua_getgcthreshold(ls);
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
        // Nullifying "this"
        lua_pushstring(ls,"this"),
        lua_pushnil(ls);
        lua_settable(ls,LUA_GLOBALSINDEX);

        push_actor_table( ls , current_actor );
        
        // Saving _LOADED variables into virtual environment, then cleaning _G[_LOADED]
        lua_pushstring(ls,"_LOADED");
        lua_pushstring(ls,"_LOADED");
        lua_gettable(ls,LUA_GLOBALSINDEX);
        lua_settable(ls,-3);
        lua_pushstring(ls,"_LOADED");
        lua_newtable(ls);
        lua_settable(ls,LUA_GLOBALSINDEX);
        
        lua_pop(ls,1);
    }
    current_actor = new_actor;
    if( current_actor ){

        push_actor_table( ls , current_actor );
        
        lua_pushstring(ls,"this");
        lua_pushstring(ls,"this");
        lua_gettable(ls,-3);
        lua_settable(ls,LUA_GLOBALSINDEX);

        // Loading virtual _LOADED table into the real _LOADED table
        lua_pushstring(ls,"_LOADED");
        lua_pushstring(ls,"_LOADED");
        lua_gettable(ls,-3);
        lua_settable(ls,LUA_GLOBALSINDEX);

        lua_pop(ls,1);
    }
}
void            LuaEngine::push_actor_table( lua_State* ls, SceneNode* actor){
    lua_pushstring(ls,"_SAUCER");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    lua_pushstring(ls,"_NODES");
    lua_gettable(ls,-2);
    lua_pushnumber(ls,actor->get_saucer_id());
    lua_gettable(ls,-2);
    lua_insert(ls,-3);
    lua_pop(ls,2);
}
void            LuaEngine::push_actor_userdata( lua_State* ls, SceneNode* actor){
    push_actor_table(ls,actor);
    lua_pushstring(ls,"this");
    lua_gettable(ls,-2);
    lua_insert(ls,-2);
    lua_pop(ls,1);
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
        return [](lua_State* ls ){
            UNUSED(ls);
            saucer_err( "This function should not exist!" );
            return 0; 
        };
    } else return (function_find->second);
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

            // Functions starting with "_" are registered as callbacks. This way we dont need to check
            // in lua every time if such script has a callback 
            if( lua_isfunction(ls,-1) && key[0] == '_' )
                script->existent_callbacks.insert( key.substr(1) );
            

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
void            LuaEngine::destroy_actor_env( SceneNode* actor ){
    SAUCER_ASSERT(actor,"Trying to destroy a Lua actor env but actor is null.");
    if(current_actor==actor) change_current_actor_env(nullptr);
    lua_pushstring(ls,"_SAUCER");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    lua_pushstring(ls,"_NODES");
    lua_gettable(ls,-2);
    lua_pushnumber(ls,actor->get_saucer_id());
    lua_pushnil(ls);
    lua_settable(ls,-3);
    lua_pop(ls,2);
}
void            LuaEngine::execute_callback( const char* callback_name , SceneNode* actor ){
    if( actor->get_script() == NULL ) return;
    SceneNode* old_actor = current_actor;
    change_current_actor_env( actor );
    push_actor_userdata( ls , actor );
    lua_pushstring( ls , (std::string("_")+std::string(callback_name)).c_str() );
    lua_gettable(ls,-2);
    lua_insert(ls,-2); // Stack now is: [the function][the actor userdata]
    lua_pop(ls,1); // Stack now is: [the function]
    if( !lua_isnil(ls,-1) ){
        int err = lua_pcall(ls,0,0,0);
        if(err)saucer_err("Error during " , callback_name , " callback");
        print_error(err,actor->get_script());
    } else lua_pop(ls,1);
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

#ifdef SAUCER_EDITOR
void    LuaEngine::push_editor_items( bool filter_out_functions , bool only_show_actors ){
    
    if( lua_gettop(ls)== 0 ){
        if(only_show_actors){
            lua_pushstring(ls,"_SAUCER");
            lua_gettable(ls,LUA_GLOBALSINDEX);
            lua_pushstring(ls,"_NODES");
            lua_gettable(ls,-2);
            lua_remove(ls,-2);
        } else {
            lua_pushstring(ls,"_G");
            lua_gettable(ls,LUA_GLOBALSINDEX); 
        }
    }
    if( lua_isnil(ls,-1) ){
        saucer_err("push_editor_items on a nil value");
        lua_pop(ls,1);
        return;
    }

    lua_pushnil(ls);
    while( lua_next(ls,-2) ){
        if( filter_out_functions && lua_isfunction(ls,-1) ){
            lua_pop(ls,1);
            continue;
        }
        int key_type = lua_type(ls,-2);
        int value_type = lua_type(ls,-1);
        std::string key;
        if( key_type == LUA_TSTRING ) key = lua_tostring(ls,-2);
        else key = std::to_string( (int)lua_tonumber(ls,-2) );
        std::string value_type_str = lua_typename(ls,value_type);
        std::string value;
        switch( value_type ){
            case LUA_TNUMBER: value = std::to_string( lua_tonumber(ls,-1) ); break;
            case LUA_TSTRING: value = lua_tostring(ls,-1); break;
            case LUA_TTABLE: value = ""; break;
            case LUA_TFUNCTION: value = ""; break;
            case LUA_TUSERDATA: value = ""; break;
            case LUA_TBOOLEAN: value = ( lua_toboolean(ls,-1) ) ? ("true") : ("false"); break;
            default: value = "?";
        }

        char line[127];
        if(only_show_actors){
            SceneNode* actor = static_cast<SceneNode*>( SaucerObject::from_saucer_id(lua_tonumber(ls,-2)) );
            sprintf(line,"%s: [%s]\t%s" , actor->get_name().c_str() , value_type_str.c_str() , value.c_str() );
        }
        else
            sprintf(line,"%s: [%s]\t%s" , key.c_str() , value_type_str.c_str() , value.c_str() );

        if( value_type == LUA_TTABLE ){
            if( ImGui::TreeNodeEx( key.c_str(), ImGuiTreeNodeFlags_SpanFullWidth , "%s" , line ) ){
                lua_pushvalue(ls,-1);
                push_editor_items(filter_out_functions);
                ImGui::TreePop();
            }
        } else {
            ImGui::BulletText( "%s" , line );
        }
    
        lua_pop(ls,1);
    }

    lua_pop(ls,1);

}
void    LuaEngine::push_actor_items( SceneNode* actor , bool filter_out_functions ){
    if( actor->get_script() ){
        push_actor_table(ls,actor);
        push_editor_items( filter_out_functions , false );
    }
}

#endif