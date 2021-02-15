#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include "resources.h"

class ProjectConfig : public Resource {
    REGISTER_SAUCER_OBJECT(ProjectConfig,Resource)

    private:
        std::string     project_name;
        Vector2         game_window_size;
        Vector2         editor_window_size;
        std::string     initial_root_path;
        bool            start_fullscreen;
        bool            window_resizable;  
    public:
        ProjectConfig();
        ProjectConfig( const std::vector<uint8_t> mem_data );

        std::string     get_project_name();
        void            set_project_name( std::string new_val );
        
        Vector2         get_game_window_size();
        void            set_game_window_size( Vector2 new_val );
        
        Vector2         get_editor_window_size();
        void            set_editor_window_size( Vector2 new_val );
        
        std::string     get_initial_root_path();
        void            set_initial_root_path( std::string new_val );
        
        bool            get_start_fullscreen();
        void            set_start_fullscreen( bool new_val );
        
        bool            get_window_resizable();
        void            set_window_resizable( bool new_val );

        YamlNode        to_yaml_node() const ;
        void            from_yaml_node( YamlNode );
          


};

#endif