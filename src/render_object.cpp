#include "render_object.h"
#include "resources/shader.h"
#include "lua_engine.h"

std::unordered_multimap< SaucerId , RenderObject* > RenderObject::component_from_node;


RenderObject::RenderObject(){
    current_shader = nullptr;
    use_parent_shader = false;
}
std::vector<RenderData>  RenderObject::generate_render_data() const{
    return std::vector<RenderData>();
}
bool            RenderObject::get_use_parent_shader() const {
    return use_parent_shader;
}
void            RenderObject::set_use_parent_shader( bool new_val ){
    use_parent_shader = new_val;
}
ShaderResource* RenderObject::get_current_shader() const {
    if( use_parent_shader ){
        SceneNode* parent = attached_node->get_parent();
        if( parent ){
            RenderObject* parent_render = parent->get_component<RenderObject>();
            if( parent_render )
                return parent_render->get_current_shader();
        }
        return nullptr;
    } else return current_shader;
}
void            RenderObject::set_current_shader( ShaderResource* p ){
    current_shader = p;
}
void     RenderObject::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( RenderObject , get_use_parent_shader );
    REGISTER_LUA_MEMBER_FUNCTION( RenderObject , set_use_parent_shader );
    REGISTER_LUA_MEMBER_FUNCTION( RenderObject , get_current_shader );
    REGISTER_LUA_MEMBER_FUNCTION( RenderObject , set_current_shader );    
}
