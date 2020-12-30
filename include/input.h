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
            operator    InputEventKey(){ return input_event_key;}
            operator    InputEventMouseButton(){ return input_event_mouse_button;}
            operator    InputEventMouseMotion(){ return input_event_mouse_motion;}
        };


    private:
        // Singleton ---------------------------
        Input();
        Input( const Input& other );
        void operator= (const Input& other);
        static      Input* input_singleton;
        // -------------------------------------

        bool        key_pressed[GLFW_KEY_LAST+1];
        bool        mouse_pressed[ 3 ];
        Vector2     window_mouse_position;
        std::vector<Input::InputEvent*> event_queue;
    
    public:
        static      Input*  instance();

        bool        is_key_pressed( int key_unicode ) const {return key_pressed[key_unicode];};
        bool        is_mouse_button_pressed( INPUT_EVENT_MOUSE_BUTTON mouse_button ) const{return mouse_pressed[mouse_button];};
        Vector2     get_window_mouse_position() const {return window_mouse_position;};

        InputEvent* pop_event_queue();    
        
        static void mouse_pos_callback( GLFWwindow* glfw_window , double x_pos , double y_pos );
        static void key_callback( GLFWwindow* glfw_window , int key, int scancode, int action, int mods );
        static void mouse_button_callback( GLFWwindow* glfw_window , int button , int action , int mods );
};

#endif