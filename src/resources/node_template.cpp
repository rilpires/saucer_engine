#include "resources/node_template.h"
#include "scene_node.h"
#include "lua_engine.h"

NodeTemplateResource::NodeTemplateResource(){
    // SceneNode::pack_as_resource
}
NodeTemplateResource::NodeTemplateResource( const std::vector<uint8_t>& data ) {
    std::string content;
    content.resize(data.size());
    memcpy( &(content[0]) , &(data[0]) , content.size() );
    yaml_node = YAML::Load( content );
}
SceneNode*  NodeTemplateResource::instantiate_node() const{
    SceneNode* ret = new SceneNode();
    ret->from_yaml_node( yaml_node );
    return ret;
}
const YamlNode  NodeTemplateResource::get_yaml_node(){
    return yaml_node;
}
void        NodeTemplateResource::save( std::string path ) const{
    std::ofstream ofs;
    ofs.open(path, std::ofstream::out );
    ofs << yaml_node;
    ofs.close();
}
void NodeTemplateResource::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION(NodeTemplateResource,instantiate_node);
    REGISTER_LUA_MEMBER_FUNCTION(NodeTemplateResource,save);
}
