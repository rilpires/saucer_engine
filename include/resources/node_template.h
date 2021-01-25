#ifndef NODE_TEMPLATE_H
#define NODE_TEMPLATE_H

#include "resources.h"
#include "file.h"

class SceneNode;

class NodeTemplateResource : public Resource {
    REGISTER_SAUCER_OBJECT(NodeTemplateResource,Resource);

    friend class SceneNode;    
    private:
        YamlNode    yaml_node;
        NodeTemplateResource();
    public:
        NodeTemplateResource( std::string path );

        SceneNode*  instantiate_node() const;
        void        save( std::string path ) const;

        static void bind_methods();
};

#endif