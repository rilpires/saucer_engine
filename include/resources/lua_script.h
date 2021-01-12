#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "resources.h"
#include "lua_engine.h"
#include <set>

class LuaScriptResource : public Resource {
    REGISTER_SAUCER_OBJECT(LuaScriptResource,Resource)
    
    friend class LuaEngine;

    private:
        std::string src;
        std::set<std::string> existent_callbacks;
    public:
        LuaScriptResource( std::string filepath );
        ~LuaScriptResource();

        const std::string&          get_src() const {return src;}
        bool                        has_callback( std::string callback_name ) const ;  
        static LuaScriptResource*   get_resource( std::string filepath );

        static void                 bind_methods();


};


#endif