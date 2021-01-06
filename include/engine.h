#ifndef WINDOW_H

#define WINDOW_H



#include <iostream>
#include <list>

#include "saucer_object.h"
#include "vector.h"

class Scene;
class AudioEngine;
class RenderEngine;

class Engine : public SaucerObject{
    REGISTER_SAUCER_OBJECT(Engine,SaucerObject);
    
    private:
        static RenderEngine*        render_engine;
        static AudioEngine*         audio_engine;
        static Scene*               current_scene;
        static std::list<double>    last_uptimes;
        
    public:
        static void             initialize( );
        static void             close();
        static void             update();
        static double           get_uptime();
        static double           get_fps();
        static double           get_last_frame_duration();
        static void             set_current_scene(Scene* scene);
        static Scene*           get_current_scene();
        static void             set_window_size( Vector2 new_size );
        static Vector2          get_window_size();
        static void             set_window_pos( Vector2 new_pos );
        static Vector2          get_window_pos();
        static void             set_fullscreen( bool fs );
        static bool             is_fullscreen();
        static std::string      get_window_title();
        static void             set_window_title( std::string new_title );
        static RenderEngine*    get_render_engine();
        static AudioEngine*     get_audio_engine();
        static bool             should_close();

        static void         bind_methods();

};

#endif