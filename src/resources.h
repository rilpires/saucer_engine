#ifndef RESOURCES_H
#define RESOURCES_H

#include <string>
#include <unordered_map>
#include "color.h"
#include "vector.h"

typedef unsigned int ResourceId;
typedef unsigned int TextureId;

class ResourceManager;
class Resource {
    friend class ResourceManager;
    protected:
        static  ResourceId  open_resource_id;
        ResourceId          resource_id;
        std::string         path;
        Resource( std::string filepath );
    public:
        Resource();
        ~Resource();
        ResourceId          get_resource_id() const {return resource_id;};
        std::string         get_path() const {return path;};
};

class ImageResource : public Resource {
    friend class ResourceManager;
    private:
        unsigned char*          data;
        size_t                  width,height;
        TextureId               tex_id;
        ImageResource( std::string filepath );
        ~ImageResource();
    public:
        TextureId               get_texture_id() const {return tex_id;}
        Color                   get_pixel(size_t x , size_t y) const;
        Vector2                 get_size() const {return Vector2(width,height);};
        static ImageResource*   get_resource(std::string p_resource_path);
};

class NumberResource : public Resource {
    public:
        double n;
        NumberResource( double number ){n=number;}
        operator int() const {return (int)n;}
        operator unsigned int() const {return (unsigned int)n;}
        static NumberResource*  get_resource(std::string p_resource_path);
};

class ResourceManager {
    private:
        static ResourceManager* singleton;
        
        std::unordered_map<ResourceId,Resource*> resources;
        std::unordered_map<std::string,ResourceId> id_by_path;

        ResourceManager();
        ResourceManager(const ResourceManager&);
        void operator=(const ResourceManager& rm);
        ~ResourceManager();
        static ResourceManager* instance();

        static Resource*    load_resource(std::string filepath);
    public:
    
        static Resource*    get_resource(ResourceId p_id);
        static Resource*    get_resource(std::string p_resource_path);
        static void         set_resource(std::string resource_name , Resource* r );
        static void         free_resource(Resource* p_resource);
};

#endif