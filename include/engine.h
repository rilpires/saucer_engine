#ifndef WINDOW_H

#define WINDOW_H

#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <list>
#include "saucer_object.h"


class Vector2;
class Input;
class Scene;


class Engine : public SaucerObject{
    REGISTER_SAUCER_OBJECT(Engine,SaucerObject);
    
    private:
        static GLFWwindow*          glfw_window;
        static Scene*               current_scene;
        static std::list<double>    last_uptimes;
        static void                 setup_renderer();

    public:
        static void         initialize( Vector2 p_size , Vector2 p_pos , const char* title );
        static void         close();
        static void         update();
        static double       get_uptime();
        static double       get_fps();
        static double       get_last_frame_duration();
        static void         set_current_scene(Scene* scene);
        static Scene*       get_current_scene();
        static void         set_window_size( Vector2 new_size );
        static Vector2      get_window_size();
        static void         set_window_pos( Vector2 new_pos );
        static Vector2      get_window_pos();
        static void         set_fullscreen( bool fs );
        static bool         is_fullscreen();
        static bool         should_close();

        static void         bind_methods();


};

#endif