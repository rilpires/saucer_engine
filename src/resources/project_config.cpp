#include "project_config.h"

ProjectConfig::ProjectConfig(){
    project_name = "Saucer project";
    game_window_size = Vector2(640,480);
    editor_window_size = Vector2(640,480);
    initial_root_path = "";
    start_fullscreen = false;
    window_resizable = false;
}
ProjectConfig::ProjectConfig( const std::vector<uint8_t> mem_data ) : ProjectConfig() {
    YamlNode yaml_node = YAML::Load( (const char*)&mem_data[0]);
    from_yaml_node(yaml_node);
}

std::string     ProjectConfig::get_project_name(){
    return project_name;
}
void            ProjectConfig::set_project_name( std::string new_val ){
    project_name = new_val;
}
Vector2         ProjectConfig::get_game_window_size(){
    return game_window_size;
}
void            ProjectConfig::set_game_window_size( Vector2 new_val ){
    game_window_size = new_val;
}
Vector2         ProjectConfig::get_editor_window_size(){
    return editor_window_size;
}
void            ProjectConfig::set_editor_window_size( Vector2 new_val ){
    editor_window_size = new_val;
}
std::string     ProjectConfig::get_initial_root_path(){
    return initial_root_path;
}
void            ProjectConfig::set_initial_root_path( std::string new_val ){
    initial_root_path = new_val;
}
bool            ProjectConfig::get_start_fullscreen(){
    return start_fullscreen;
}
void            ProjectConfig::set_start_fullscreen( bool new_val ){
    start_fullscreen = new_val;
}
bool            ProjectConfig::get_window_resizable(){
    return window_resizable;
}
void            ProjectConfig::set_window_resizable( bool new_val ){
    window_resizable = new_val;
}

YamlNode        ProjectConfig::to_yaml_node() const {
    YamlNode ret;
    ret["project_name"] = project_name;
    ret["game_window_size"] = game_window_size;
    ret["editor_window_size"] = editor_window_size;
    ret["initial_root_path"] = initial_root_path;
    ret["start_fullscreen"] = start_fullscreen;
    ret["window_resizable"] = window_resizable;
    return ret;
}
void            ProjectConfig::from_yaml_node( YamlNode yaml_node ){
    if( yaml_node["project_name"].IsDefined() )         project_name       = yaml_node["project_name"].as<decltype(project_name)>();
    if( yaml_node["game_window_size"].IsDefined() )     game_window_size   = yaml_node["game_window_size"].as<decltype(game_window_size)>();
    if( yaml_node["editor_window_size"].IsDefined() )   editor_window_size = yaml_node["editor_window_size"].as<decltype(editor_window_size)>();
    if( yaml_node["initial_root_path"].IsDefined() )    initial_root_path  = yaml_node["initial_root_path"].as<decltype(initial_root_path)>();
    if( yaml_node["start_fullscreen"].IsDefined() )     start_fullscreen   = yaml_node["start_fullscreen"].as<decltype(start_fullscreen)>();
    if( yaml_node["window_resizable"].IsDefined() )     window_resizable   = yaml_node["window_resizable"].as<decltype(window_resizable)>();
}
        
