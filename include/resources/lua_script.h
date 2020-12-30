#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "resources.h"
#include <string>

class LuaScriptResource : public Resource {
    private:
        std::string src;
    public:
        LuaScriptResource( std::string filepath );
        ~LuaScriptResource();

        const std::string&          get_src() const {return src;}

        static LuaScriptResource*   get_resource( std::string filepath );

        static void                 bind_methods();


};


#endif