#ifndef INPUT_H
#define INPUT_H

#include "engine.h" // glfw events
#include <vector>  // std
#include "vector.h" // Vector2
#include <iostream>

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
            short                       key_unicode;
            INPUT_EVENT_ACTION          action;
        };
        struct InputEventMouseButton {
            INPUT_EVENT_TYPE            type;
            bool                        is_solved;
            INPUT_EVENT_MOUSE_BUTTON    mouse_button;
            INPUT_EVENT_ACTION          action;
        };
        struct InputEventMouseMotion {
            INPUT_EVENT_TYPE            type;
            bool                        is_solved;        
            float                       window_x,window_y;
        };

        union InputEvent {
            InputEventKey           input_event_key;
            InputEventMouseButton   input_event_mouse_button;
            InputEventMouseMotion   input_event_mouse_motion;
            
            INPUT_EVENT_TYPE    get_type(){return input_event_key.type;}
            std::string         get_type_str() const { 
                switch(input_event_key.type){
                    case INPUT_EVENT_TYPE_KEY: return "key";
                    case INPUT_EVENT_TYPE_MOUSE_BUTTON: return "mouse_button";
                    case INPUT_EVENT_TYPE_MOUSE_MOTION: return "mouse_motion";
                } return "call the cops";
            }
            void                solve(){ input_event_key.is_solved = true; }
            bool                is_solved() const { return input_event_key.is_solved; }
            Vector2             get_mouse_position() const {return Vector2(input_event_mouse_motion.window_x,input_event_mouse_motion.window_y);};
            
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
        
        static bool         is_key_pressed( int key_unicode ) {return key_pressed[key_unicode];};
        static bool         is_mouse_button_pressed( INPUT_EVENT_MOUSE_BUTTON mouse_button ){return mouse_pressed[mouse_button];};
        static Vector2      get_window_mouse_position() {return window_mouse_position;};

        static InputEvent*  pop_event_queue();    
        
        static void         mouse_pos_callback( GLFWwindow* glfw_window , double x_pos , double y_pos );
        static void         key_callback( GLFWwindow* glfw_window , int key, int scancode, int action, int mods );
        static void         mouse_button_callback( GLFWwindow* glfw_window , int button , int action , int mods );


        static void         bind_methods();

};

#endif