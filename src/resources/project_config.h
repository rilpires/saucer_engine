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
        Color           clear_color;  
    public:
        ProjectConfig();
        ProjectConfig( const std::vector<uint8_t> mem_data );

        std::string     get_project_name() const ;
        void            set_project_name( std::string new_val );
        
        Vector2         get_game_window_size() const ;
        void            set_game_window_size( Vector2 new_val );
        
        Vector2         get_editor_window_size() const ;
        void            set_editor_window_size( Vector2 new_val );
        
        std::string     get_initial_root_path() const ;
        void            set_initial_root_path( std::string new_val );
        
        bool            get_start_fullscreen() const ;
        void            set_start_fullscreen( bool new_val );
        
        bool            get_window_resizable() const ;
        void            set_window_resizable( bool new_val );

        Color           get_clear_color() const ;
        void            set_clear_color( Color new_val );

        YamlNode        to_yaml_node() const ;
        void            from_yaml_node( YamlNode );
          


};

#endif