#include "input.h"

Input* Input::input_singleton = NULL;

Input* Input::instance(){;
    if( input_singleton == NULL ) input_singleton = new Input();
    return input_singleton;
}

Input::Input(){
    for( auto& i : key_pressed ) i = false;
    for( auto& i : mouse_pressed ) i = false;
    window_mouse_position = Vector2(0,0);
}

Input::InputEvent* Input::pop_event_queue(){
    if( event_queue.size() > 0 ){
        auto ret = event_queue.front();
        event_queue.erase(event_queue.begin());
        return ret;
    } else return NULL;
}
void Input::mouse_pos_callback( GLFWwindow* glfw_window , double x_pos , double y_pos ){
    instance()->window_mouse_position = Vector2(x_pos,y_pos);
    Input::InputEvent* new_input_event = new Input::InputEvent();
    new_input_event->input_event_mouse_motion.is_solved = false;
    new_input_event->input_event_mouse_motion.type = INPUT_EVENT_TYPE_MOUSE_MOTION;
    new_input_event->input_event_mouse_motion.window_x = x_pos;
    new_input_event->input_event_mouse_motion.window_y = y_pos;
    // If last event queued is a mouse motion, it is discarded, since it hasn't been solved yet and we already got a new one
    if( !instance()->event_queue.empty() && instance()->event_queue.back()->input_event_mouse_motion.type == INPUT_EVENT_TYPE_MOUSE_MOTION ){
        delete instance()->event_queue.back();
        instance()->event_queue.pop_back();
    }
    instance()->event_queue.push_back( new_input_event );
}
void Input::key_callback( GLFWwindow* glfw_window , int key, int scancode, int action, int mods ){
    instance()->key_pressed[key] = (action!=RELEASED);
    Input::InputEvent* new_input_event = new Input::InputEvent();
    new_input_event->input_event_key.is_solved = false;
    new_input_event->input_event_key.type = INPUT_EVENT_TYPE_KEY;
    new_input_event->input_event_key.key_unicode = key;
    new_input_event->input_event_key.action = (INPUT_EVENT_ACTION)action;
    instance()->event_queue.push_back( new_input_event );
}
void Input::mouse_button_callback( GLFWwindow* glfw_window , int button , int action , int mods ){
    instance()->mouse_pressed[button] = (action!=RELEASED);
    Input::InputEvent* new_input_event = new Input::InputEvent();
    new_input_event->input_event_mouse_button.is_solved = false;
    new_input_event->input_event_mouse_button.type = INPUT_EVENT_TYPE_MOUSE_BUTTON;
    new_input_event->input_event_mouse_button.mouse_button = (INPUT_EVENT_MOUSE_BUTTON)button;
    new_input_event->input_event_mouse_button.action = (INPUT_EVENT_ACTION)action;
    instance()->event_queue.push_back( new_input_event );
}