#include "lua_engine.h"

#include <string.h>
#include "scene_node.h"
#include <iostream>
#include <set>

lua_State*  LuaEngine::ls = NULL;
int         LuaEngine::kb_memory_used = 0;
int         LuaEngine::kb_memory_threshold = 0;
size_t      LuaEngine::chunk_reader_offset = 0;
SceneNode*  LuaEngine::current_actor = NULL;

const char* LuaEngine::chunk_reader( lua_State* ls , void* data , size_t* size ){
    size_t total_size = strlen((char*)data);
    if( chunk_reader_offset < total_size ){
        size_t remaining_size = total_size - chunk_reader_offset;
        *size = ( remaining_size < CHUNK_READER_SIZE )?(remaining_size):(CHUNK_READER_SIZE) ;
        char* ret = (char*)(data+chunk_reader_offset);
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

    #define SAUCER_DEFINE_GLOBAL_LUA_FUNCTION(s) \
        lua_pushstring(ls , #s ); \
        lua_pushcfunction(ls , s ); \
        lua_settable(ls,LUA_GLOBALSINDEX);
    
    SAUCER_DEFINE_GLOBAL_LUA_FUNCTION( get_position );
    SAUCER_DEFINE_GLOBAL_LUA_FUNCTION( set_position );
    SAUCER_DEFINE_GLOBAL_LUA_FUNCTION( get_parent );

}
void        LuaEngine::change_current_actor_env( SceneNode* new_actor ){
    if( current_actor == new_actor ) return;
    if( current_actor ){
        lua_pushfstring(ls,"_SAUCER");
        lua_gettable(ls,LUA_GLOBALSINDEX);
        lua_pushfstring(ls,"_NODES");
        lua_gettable(ls,-2);
        lua_pushnumber(ls,current_actor->get_id());
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
        lua_pushnumber(ls,current_actor->get_id());
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
        if( ls,lua_type(ls,i) == LUA_TNUMBER ) 
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


int         LuaEngine::get_parent( lua_State* ls ){
    SceneNodeId id = ( lua_gettop(ls) > 0 )?(*(SceneNodeId*)lua_touserdata(ls,1)):(current_actor->get_id());
    lua_pop(ls,1);
    SceneNode* parent = SceneNode::from_id(id)->get_parent_node();
    lua_push_scene_node(ls,parent);
    return 1;
}
int         LuaEngine::get_position( lua_State* ls ){
    SceneNodeId id = ( lua_gettop(ls) > 0 )?(*(SceneNodeId*)lua_touserdata(ls,1)):(current_actor->get_id());
    lua_pop(ls,1);
    lua_push_vector2(ls,SceneNode::from_id(id)->get_position());
    return 1;
}
int         LuaEngine::set_position( lua_State* ls ){
    SceneNodeId id = ( lua_gettop(ls) > 1 )?(*(SceneNodeId*)lua_touserdata(ls,1)):(current_actor->get_id());
    Vector2* v = (Vector2*)lua_touserdata(ls,-1);
    lua_pop(ls,2);
    SceneNode::from_id(id)->set_position(*v);
    return 0;
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

    lua_pushstring(ls,"_NODE_ID");
    lua_pushnumber(ls,new_actor->get_id());
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
    lua_pushstring(ls,"_NODE_USERDATA");
    lua_push_scene_node( ls , new_actor );
    lua_settable(ls,-3);

    // Now inserting it into _G["_SAUCER"]["_NODES"][id]
    lua_pushstring(ls,"_SAUCER");
    lua_gettable(ls,LUA_GLOBALSINDEX);
    lua_pushstring(ls,"_NODES");
    lua_gettable(ls,-2);
    lua_pushnumber(ls,new_actor->get_id());
    lua_pushvalue(ls,1);
    lua_settable(ls,-3);
    lua_pop(ls,3);

    change_current_actor_env(old_actor);
}

void        LuaEngine::lua_push_scene_node( lua_State* ls , SceneNode* node ){
    if( node )  *(SceneNodeId*) lua_newuserdata(ls,sizeof(SceneNodeId)) = node->get_id();
    else        *(SceneNodeId*) lua_newuserdata(ls,sizeof(SceneNodeId)) = 0;
        
    lua_newtable(ls);
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls,[](lua_State* ls){
        const char* arg = lua_tostring(ls,-1);
        SceneNodeId id = *(SceneNodeId*)lua_touserdata(ls,-2);
        lua_pop(ls,2);
                if( strcmp(arg,"get")==0 ) lua_pushnumber(ls,555);
        else    if( strcmp(arg,"get_position")==0 ) lua_pushcfunction(ls,get_position) ;
        else    if( strcmp(arg,"set_position")==0 ) lua_pushcfunction(ls,set_position) ;
        else    if( strcmp(arg,"get_parent")==0 )   lua_pushcfunction(ls,get_parent) ;
        return 1;
    });

    lua_settable(ls,-3);
    lua_setmetatable(ls,-2);
}
void        LuaEngine::lua_push_vector2( lua_State* ls , Vector2 v ){
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

    lua_setmetatable(ls,-2);

}
