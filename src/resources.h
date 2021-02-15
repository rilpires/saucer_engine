#ifndef RESOURCES_H
#define RESOURCES_H

#include "debug.h"
#include "file.h"
#include <unordered_map>
#include "vector.h"
#include "saucer_object.h"
#include "yaml-cpp/yaml.h"
#include <fstream>

class ResourceManager;
class Resource : public SaucerObject {
    REGISTER_SAUCER_OBJECT(Resource,SaucerObject)
    
    friend class ResourceManager;
    protected:
        std::string         path;
        bool                dirty;
        Resource();
    public:
        ~Resource();
        std::string         get_path() const ;
        
        /**
         * @brief By default, this doesn't set the dirty flag as true. If a resource should in fact be reloaded, it should override this function to set dirty as true
         */
        virtual void        flag_as_dirty();
        /**
         * @brief By default, doesn't do anything.
         * 
         * @return true when succesfully reloaded
         * @return false when not succesfully reloaded
         */
        virtual bool        reload();

        static void         bind_methods();   
};


class ResourceManager : public SaucerObject {
    REGISTER_SAUCER_OBJECT(ResourceManager,SaucerObject);
    
    public:
        struct ContentTableEntry{
            friend class ResourceManager;
            private:
            ContentTableEntry();
            char            path[256];
            uint64_t        offset;
            uint64_t        compressed_size;
            
            public:
            ContentTableEntry( std::string p_path , uint64_t p_offset , uint64_t p_compressed_size );
            std::string     get_path();
            const uint64_t  get_offset();
            const uint64_t  get_compressed_size();
        };

    private:
        static std::unordered_map<std::string,SaucerObject::SaucerId> id_by_path;
        static std::unordered_map<std::string,ContentTableEntry> toc_entries;
        static std::ifstream package_stream;

        static Resource*            load_resource(std::string filepath);
    public:
        static void                 fetch_package(std::string package_filepath = "package.data" );
        static std::vector<uint8_t> get_data( std::string filepath );
        template<typename T = Resource>
        static T*                   get_resource(std::string p_resource_path);
        static void                 set_resource(std::string resource_name , Resource* r );
        static void                 free_resource(Resource* p_resource);
        static void                 dirty_every_resource();
        
        static const decltype(id_by_path)::iterator begin();
        static const decltype(id_by_path)::iterator end();         

        static void         bind_methods();
};

template<typename T>
T*  ResourceManager::get_resource(std::string p_resource_path){
    if( p_resource_path.size()==0 ) return nullptr;
    return static_cast<T*>(get_resource<Resource>(p_resource_path));
}
template<> Resource* ResourceManager::get_resource(std::string p_resource_path);
        

#endif