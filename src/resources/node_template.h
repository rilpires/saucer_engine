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
        NodeTemplateResource( );
    public:
        NodeTemplateResource( const std::vector<uint8_t>& data );

        SceneNode*      instantiate_node() const;
        const YamlNode  get_yaml_node();
        void            save( std::string path ) const;

        static void bind_methods();
};

#endif