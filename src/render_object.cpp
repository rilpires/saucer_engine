#include "render_object.h"
#include "resources/shader.h"
#include "lua_engine.h"

RenderObject::RenderObject(){
    current_shader = nullptr;
    use_parent_shader = true;
}
RenderObject::~RenderObject(){
}
std::vector<RenderData>  RenderObject::generate_render_data(){
    return std::vector<RenderData>();
}
bool            RenderObject::get_use_parent_shader() const {
    return use_parent_shader;
}
void            RenderObject::set_use_parent_shader( bool new_val ){
    use_parent_shader = new_val;
}
ShaderResource* RenderObject::get_current_shader() const {
    ShaderResource* ret = nullptr;
    if( use_parent_shader ){
        SceneNode* parent = attached_node->get_parent();
        RenderObject* next_parent_render = nullptr;
        while( 1 ){
            if( parent ) next_parent_render = parent->get_component<RenderObject>(); 
            if( next_parent_render ){
                ret = next_parent_render->get_current_shader();
                break;
            } else {
                if( parent ){
                    parent = parent->get_parent();
                } 
                else {
                    ret = current_shader; 
                    break;
                }
            }
        }
    } else ret = current_shader;
    
    return (ret)?(ret):(Engine::get_render_engine()->get_basic_shader());
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
