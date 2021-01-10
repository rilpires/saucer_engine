#ifndef RESOURCES_H
#define RESOURCES_H

#include "debug.h"
#include <unordered_map>
#include "vector.h"
#include "saucer_object.h"

class ResourceManager;
class Resource : public SaucerObject {
    REGISTER_SAUCER_OBJECT(Resource,SaucerObject)
    
    friend class ResourceManager;
    protected:
        std::string         path;
        Resource( std::string filepath );
    public:
        Resource();
        ~Resource();
        std::string         get_path() const {return path;};


        static std::string  read_file_as_str( std::string filename );
        static void         bind_methods();   
};


class ResourceManager {
    private:        
        static std::unordered_map<std::string,SaucerObject::SaucerId> id_by_path;
        
        static Resource*    load_resource(std::string filepath);
    public:
        static Resource*    get_resource(std::string p_resource_path);
        static void         set_resource(std::string resource_name , Resource* r );
        static void         free_resource(Resource* p_resource);

        static void         bind_methods();
};

#endif