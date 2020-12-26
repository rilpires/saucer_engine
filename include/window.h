#ifndef WINDOW_H

#define WINDOW_H

#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "saucer_object.h"


class Vector2;
class Input;
class Scene;


class Window : public SaucerObject{
    private:
        GLFWwindow* glfw_window;
        Scene*      current_scene;

        void        setup_renderer() const;

    public:
        Window( Vector2 p_size , Vector2 p_pos , const char* title );
        ~Window();

        void        update();

        void        set_current_scene(Scene* scene);
        Scene*      get_current_scene() const { return current_scene;}

        void        set_window_size( Vector2 new_size );
        Vector2     get_window_size() const ;

        void        set_window_pos( Vector2 new_pos );
        Vector2     get_window_pos() const ;

        void        set_fullscreen( bool fs );
        bool        is_fullscreen();

        bool        should_close() const;


};

#endif