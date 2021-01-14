#ifndef INPUT_H
#define INPUT_H

#include "render_engine.h" // glfw events
#include <vector>  // std
#include "vector.h" // Vector2
#include "debug.h"
#include <list>

enum INPUT_EVENT_TYPE {         INPUT_EVENT_TYPE_KEY , 
                                INPUT_EVENT_TYPE_MOUSE_BUTTON , 
                                INPUT_EVENT_TYPE_MOUSE_MOTION };

enum INPUT_EVENT_ACTION {       PRESSED=1 , 
                                RELEASED=0,
                                ECHO=2 };

enum INPUT_EVENT_MOUSE_BUTTON { BUTTON_LEFT     = GLFW_MOUSE_BUTTON_LEFT , 
                                BUTTON_MIDDLE   = GLFW_MOUSE_BUTTON_MIDDLE , 
                                BUTTON_RIGHT    = GLFW_MOUSE_BUTTON_RIGHT };



class Input {
    
    public:

        struct InputEventKey {
            INPUT_EVENT_TYPE            type;
            bool                        is_solved;
            INPUT_EVENT_ACTION          action;
            short                       key_unicode;
        };
        struct InputEventMouseButton {
            INPUT_EVENT_TYPE            type;
            bool                        is_solved;
            INPUT_EVENT_ACTION          action;
            INPUT_EVENT_MOUSE_BUTTON    mouse_button;
        };
        struct InputEventMouseMotion {
            INPUT_EVENT_TYPE            type;
            bool                        is_solved;        
            float                       window_x,window_y;
        };

        union InputEvent {
            static constexpr const char* class_name = "InputEvent";
            InputEventKey           input_event_key;
            InputEventMouseButton   input_event_mouse_button;
            InputEventMouseMotion   input_event_mouse_motion;
            
            int                 get_type() const ;
            int                 get_button() const ;
            int                 get_key() const ;
            std::string         get_type_str() const;
            Vector2             get_mouse_position() const;
            bool                is_pressed() const;
            bool                is_echo() const;
            void                solve();
            bool                is_solved() const;
            
            operator            InputEventKey(){ return input_event_key;}
            operator            InputEventMouseButton(){ return input_event_mouse_button;}
            operator            InputEventMouseMotion(){ return input_event_mouse_motion;}
        
        };


    private:

        static bool         key_pressed[GLFW_KEY_LAST+1];
        static bool         mouse_pressed[ 3 ];
        static Vector2      window_mouse_position;
        static std::list<Input::InputEvent*> event_queue;
    
    public:
        
        static bool         is_key_pressed( int key_unicode ) ;
        static bool         is_mouse_button_pressed( int mouse_button );
        static Vector2      get_screen_mouse_position();
        static Vector2      get_world_mouse_position();
        
        static InputEvent*  pop_event_queue();    
        
        static void         mouse_pos_callback( GLFWwindow* glfw_window , double x_pos , double y_pos );
        static void         key_callback( GLFWwindow* glfw_window , int key, int scancode, int action, int mods );
        static void         mouse_button_callback( GLFWwindow* glfw_window , int button , int action , int mods );


        static void         bind_methods();

};


#endif