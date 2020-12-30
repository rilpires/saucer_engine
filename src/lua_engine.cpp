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


#define _LUAENGINE_PUSH_AS_NUMBER(t)                    \
template<> void LuaEngine::push( lua_State* ls , t n ){ \
    lua_pushnumber(ls,n);                               \
}                                                          

#define _LUAENGINE_POP_AS_NUMBER(T)                       \
template<> T    LuaEngine::pop( lua_State* ls ){    \
    T ret = lua_tonumber(ls,-1);                    \
    lua_pop(ls,1);                                  \
    return ret;                                     \
} 

#define _LUAENGINE_POP_SAUCER_OBJECT( T )                                   \
template<> T  LuaEngine::pop( lua_State* ls ){                              \
    SaucerId saucer_id = *(SaucerId*)lua_touserdata(ls,-1);                 \
    T ret = static_cast<T>( SaucerObject::from_saucer_id(saucer_id) );      \
    lua_pop(ls,1);                                                          \
    return ret;                                                             \
}

#define _LUAENGINE_POP_USERDATA_AS_VALUE( T )   \
template<> T  LuaEngine::pop( lua_State* ls ){  \
    T ret = *(T*)lua_touserdata(ls,-1);         \
    lua_pop(ls,1);                              \
    return ret;                                 \
}   

_LUAENGINE_PUSH_AS_NUMBER(double)
_LUAENGINE_PUSH_AS_NUMBER(float)
_LUAENGINE_PUSH_AS_NUMBER(int)
_LUAENGINE_PUSH_AS_NUMBER(short)

template<> void LuaEngine::push( lua_State* ls , Vector2 v ){
    void* userdata = lua_newuserdata( ls , sizeof(Vector2) );
    (*(Vector2*)userdata) = v;

    // Pushing a vector2 metatable:
    lua_newtable(ls);
    
    // __index
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        Vector2* v = (Vector2*)lua_touserdata(ls,-2);
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
                if(!strcmp(arg,"x"))   lua_pushnumber(ls,v->x);
        else    if(!strcmp(arg,"y"))   lua_pushnumber(ls,v->y);
        else    if(!strcmp(arg,"rotated"))   lua_pushcfunction(ls,c_function_db["Vector2"]["rotated"]);
        return 1;
    });
    lua_settable(ls,-3);

    // __newindex
    lua_pushstring(ls,"__newindex");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        Vector2* v = (Vector2*)lua_touserdata(ls,-3);
        const char* arg = lua_tostring(ls,-2);
        float new_val = lua_tonumber(ls,-1);
        lua_pop(ls,3);
                if(!strcmp(arg,"x"))    v->x=new_val;
        else    if(!strcmp(arg,"y"))    v->y=new_val;
        return 0;
    });
    lua_settable(ls,-3);

    
    #define PUSH_VECTOR2_METATABLE_OPERATION(index_str,operator)     \
    lua_pushstring(ls,index_str);                                    \
    lua_pushcfunction(ls, [](lua_State* ls)->int{                    \
        Vector2 v1 = *(Vector2*)lua_touserdata(ls,-2);               \
        Vector2 v2 = *(Vector2*)lua_touserdata(ls,-1);               \
        lua_pop(ls,2);                                               \
        LuaEngine::push(ls,v1 operator v2);                          \
        return 1;                                                    \
    });                                                              \
    lua_settable(ls,-3);

    #define PUSH_VECTOR2_METATABLE_OPERATION_SCALAR(index_str,operator) \
    lua_pushstring(ls,index_str);                                       \
    lua_pushcfunction(ls, [](lua_State* ls)->int{                       \
        Vector2 v1 = *(Vector2*)lua_touserdata(ls,-2);                  \
        float f = lua_tonumber(ls,-1);                                  \
        lua_pop(ls,2);                                                  \
        LuaEngine::push(ls,v1 operator f);                              \
        return 1;                                                       \
    });                                                                 \
    lua_settable(ls,-3);
    
    PUSH_VECTOR2_METATABLE_OPERATION("__add",+);
    PUSH_VECTOR2_METATABLE_OPERATION("__sub",-);
    PUSH_VECTOR2_METATABLE_OPERATION_SCALAR("__mul",*);
    PUSH_VECTOR2_METATABLE_OPERATION_SCALAR("__div",/);
    

    lua_setmetatable(ls,-2);

}
template<> void LuaEngine::push( lua_State* ls , Vector3 v ){
    void* userdata = lua_newuserdata( ls , sizeof(Vector3) );
    (*(Vector3*)userdata) = v;

    // Pushing a vector3 metatable:
    lua_newtable(ls);
    
    // __index
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        Vector3* v = (Vector3*)lua_touserdata(ls,-2);
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
                if(!strcmp(arg,"x"))   lua_pushnumber(ls,v->x);
        else    if(!strcmp(arg,"y"))   lua_pushnumber(ls,v->y);
        else    if(!strcmp(arg,"rotated"))   lua_pushcfunction(ls,c_function_db["Vector3"]["rotated"]);
        return 1;
    });
    lua_settable(ls,-3);

    // __newindex
    lua_pushstring(ls,"__newindex");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        Vector3* v = (Vector3*)lua_touserdata(ls,-3);
        const char* arg = lua_tostring(ls,-2);
        float new_val = lua_tonumber(ls,-1);
        lua_pop(ls,3);
                if(!strcmp(arg,"x"))    v->x=new_val;
        else    if(!strcmp(arg,"y"))    v->y=new_val;
        return 0;
    });
    lua_settable(ls,-3);

    
    #define PUSH_VECTOR3_METATABLE_OPERATION(index_str,operator)     \
    lua_pushstring(ls,index_str);                                    \
    lua_pushcfunction(ls, [](lua_State* ls)->int{                    \
        Vector3 v1 = *(Vector3*)lua_touserdata(ls,-2);               \
        Vector3 v2 = *(Vector3*)lua_touserdata(ls,-1);               \
        lua_pop(ls,2);                                               \
        LuaEngine::push(ls,v1 operator v2);                          \
        return 1;                                                    \
    });                                                              \
    lua_settable(ls,-3);

    #define PUSH_VECTOR3_METATABLE_OPERATION_SCALAR(index_str,operator) \
    lua_pushstring(ls,index_str);                                       \
    lua_pushcfunction(ls, [](lua_State* ls)->int{                       \
        Vector3 v1 = *(Vector3*)lua_touserdata(ls,-2);                  \
        float f = lua_tonumber(ls,-1);                                  \
        lua_pop(ls,2);                                                  \
        LuaEngine::push(ls,v1 operator f);                              \
        return 1;                                                       \
    });                                                                 \
    lua_settable(ls,-3);
    
    PUSH_VECTOR3_METATABLE_OPERATION("__add",+);
    PUSH_VECTOR3_METATABLE_OPERATION("__sub",-);
    PUSH_VECTOR3_METATABLE_OPERATION_SCALAR("__mul",*);
    PUSH_VECTOR3_METATABLE_OPERATION_SCALAR("__div",/);
    

    lua_setmetatable(ls,-2);

}
template<> void LuaEngine::push( lua_State* ls , Color v ){
    void* userdata = lua_newuserdata( ls , sizeof(Color) );
    (*(Color*)userdata) = v;

    // Pushing a vector3 metatable:
    lua_newtable(ls);
    
    // __index
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        Color* v = (Color*)lua_touserdata(ls,-2);
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
                if(!strcmp(arg,"r"))   lua_pushnumber(ls,v->r);
        else    if(!strcmp(arg,"g"))   lua_pushnumber(ls,v->g);
        else    if(!strcmp(arg,"b"))   lua_pushnumber(ls,v->b);
        else    if(!strcmp(arg,"a"))   lua_pushnumber(ls,v->a);
        else    if(!strcmp(arg,"rotated"))   lua_pushcfunction(ls,c_function_db["Color"]["rotated"]);
        return 1;
    });
    lua_settable(ls,-3);

    // __newindex
    lua_pushstring(ls,"__newindex");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        Color* v = (Color*)lua_touserdata(ls,-3);
        const char* arg = lua_tostring(ls,-2);
        float new_val = lua_tonumber(ls,-1);
        lua_pop(ls,3);
                if(!strcmp(arg,"r"))    v->r=new_val;
        else    if(!strcmp(arg,"g"))    v->g=new_val;
        else    if(!strcmp(arg,"b"))    v->b=new_val;
        else    if(!strcmp(arg,"a"))    v->a=new_val;
        return 0;
    });
    lua_settable(ls,-3);
    
    lua_setmetatable(ls,-2);
}
template<> void LuaEngine::push( lua_State* ls , bool b ){
    lua_pushboolean(ls,b);
}
template<> void LuaEngine::push( lua_State* ls , std::string s ){
    lua_pushstring(ls,s.c_str());
}
template<> void LuaEngine::push( lua_State* ls , SceneNode* obj ){
    if( obj )   *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = obj->get_saucer_id();
    else        *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = 0;
        
    lua_newtable(ls);
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls,[](lua_State* ls){
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
        lua_pushcfunction( ls , c_function_db["SceneNode"][arg] );
        return 1;
    });
    lua_settable(ls,-3);
    lua_setmetatable(ls,-2);
}
template<> void LuaEngine::push( lua_State* ls , Scene* r ){
    lua_pushnumber(ls,666);
}
template<> void LuaEngine::push( lua_State* ls , Resource* r ){
    if( r ) *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = r->get_saucer_id();
    else    *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = 0;

    lua_newtable(ls);
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls,[](lua_State* ls){
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
        lua_pushcfunction( ls , c_function_db["Resource"][arg] );
        return 1;
    });
    lua_settable(ls,-3);
    lua_setmetatable(ls,-2);
}
template<> void LuaEngine::push( lua_State* ls , ImageResource* r ){
    if( r ) *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = r->get_saucer_id();
    else    *(SaucerId*) lua_newuserdata(ls,sizeof(SaucerId)) = 0;

    lua_newtable(ls);
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls,[](lua_State* ls){
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
        lua_pushcfunction( ls , c_function_db["Resource"][arg] ); // Uhhh gotta work around this issue (member inheritance)
        return 1;
    });
    lua_settable(ls,-3);
    lua_setmetatable(ls,-2);
}
template<> void LuaEngine::push( lua_State* ls , LuaScriptResource* r ){
    lua_pushnumber(ls,666);
}
template<> void LuaEngine::push( lua_State* ls , std::vector<SceneNode*> const& v ){
    lua_newtable(ls);
    for( size_t i = 1 ; i <= v.size() ; i++ ){
        lua_pushnumber(ls,i);
        push(ls,v[i-1]);
        lua_settable(ls,-3);
    }
}

_LUAENGINE_POP_AS_NUMBER(int)
_LUAENGINE_POP_AS_NUMBER(float)
_LUAENGINE_POP_AS_NUMBER(short)
_LUAENGINE_POP_AS_NUMBER(double)
_LUAENGINE_POP_SAUCER_OBJECT(SceneNode*)   
_LUAENGINE_POP_SAUCER_OBJECT(Scene*)   
_LUAENGINE_POP_SAUCER_OBJECT(Resource*)   
_LUAENGINE_POP_SAUCER_OBJECT(ImageResource*)   
_LUAENGINE_POP_SAUCER_OBJECT(LuaScriptResource*)                                                         
_LUAENGINE_POP_USERDATA_AS_VALUE(Vector2)                                                       
_LUAENGINE_POP_USERDATA_AS_VALUE(Vector3)                                                       
_LUAENGINE_POP_USERDATA_AS_VALUE(Color)

template<> bool             LuaEngine::pop( lua_State* ls ){ 
    bool ret = lua_toboolean(ls,-1);
    lua_pop(ls,1);
    return ret;
} 
template<> std::string      LuaEngine::pop( lua_State* ls ){ 
    std::string ret = lua_tostring(ls,-1);
    lua_pop(ls,1);
    return ret; 
} 

template<> lua_CFunction    LuaEngine::create_lua_constructor<Vector2>( lua_State* ls ){
    return [](lua_State* ls){
        float x = lua_tonumber(ls,-2);
        float y = lua_tonumber(ls,-1);
        lua_pop( ls , 2 );
        LuaEngine::push( ls , Vector2(x,y) );
        return 1;
    };
}
template<> lua_CFunction    LuaEngine::create_lua_constructor<Vector3>( lua_State* ls ){
    return [](lua_State* ls){
        float r = lua_tonumber(ls,-4);
        float g = lua_tonumber(ls,-3);
        float b = lua_tonumber(ls,-2);
        float a = lua_tonumber(ls,-1);
        lua_pop( ls , 4 );
        LuaEngine::push( ls , Color(r,g,b,a) );
        return 1;
    };
}
template<> lua_CFunction    LuaEngine::create_lua_constructor<Color>( lua_State* ls ){
    return [](lua_State* ls){
        float x = lua_tonumber(ls,-3);
        float y = lua_tonumber(ls,-2);
        float z = lua_tonumber(ls,-1);
        lua_pop( ls , 3 );
        LuaEngine::push( ls , Vector3(x,y,z) );
        return 1;
    };
}

const char* LuaEngine::chunk_reader( lua_State* ls , void* data , size_t* size ){
    size_t total_size = strlen((char*)data);
    if( chunk_reader_offset < total_size ){
        size_t remaining_size = total_size - chunk_reader_offset;
        *size = ( remaining_size < CHUNK_READER_SIZE )?(remaining_size):(CHUNK_READER_SIZE) ;
        char* ret = &((char*)data)[chunk_reader_offset];
        chunk_reader_offset += CHUNK_READER_SIZE;
        return ret;
    } else return NULL;
}
void        LuaEngine::initialize(){
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
void        LuaEngine::finish(){
    lua_close(ls);
}
void        LuaEngine::create_global_env( ){
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
            else lua_pushstring(ls,function_name.c_str());
            lua_pushcfunction( ls , it2.second );
            lua_settable(ls,-3);
        }
        lua_settable(ls,LUA_GLOBALSINDEX);
    }

    LUAENGINE_VALUE_CONSTRUCTOR(Color);
    LUAENGINE_VALUE_CONSTRUCTOR(Vector2);
    LUAENGINE_VALUE_CONSTRUCTOR(Vector3);

}
void        LuaEngine::change_current_actor_env( SceneNode* new_actor ){
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
void        LuaEngine::describe_stack(){
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
void        LuaEngine::print_error( int err , LuaScriptResource* script ){   
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
        // exit(1);
    }
}
void        LuaEngine::execute_frame_start( SceneNode* actor ){
    change_current_actor_env( actor );
    lua_pushstring(ls,"frame_start");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    lua_pushnumber(ls,0.15);
    int err = lua_pcall(ls,1,0,0);

    print_error(err,actor->get_script_resource());
}
void        LuaEngine::execute_input_event( SceneNode* actor ){

}
void        LuaEngine::create_actor_env( SceneNode* new_actor ){
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
void        LuaEngine::register_function( std::string class_name , std::string function_name , lua_CFunction f ){
    c_function_db[class_name][function_name] = f;
}
