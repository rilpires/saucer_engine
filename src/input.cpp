#include "input.h"
#include "core.h"

template<> void LuaEngine::push( lua_State* ls , Input::InputEvent* r ){
    // This is unusual we will take care
    Input::InputEvent** input_pointerpointer = (Input::InputEvent**)lua_newuserdata( ls , sizeof(Input::InputEvent*) );
    *input_pointerpointer = r;


    // Pushing a vector2 metatable:
    lua_newtable(ls);
    
    // __index
    lua_pushstring(ls,"__index");
    lua_pushcfunction(ls, [](lua_State* ls)->int{
        // Input::InputEvent** ev = (Input::InputEvent**)lua_touserdata(ls,-2);
        const char* arg = lua_tostring(ls,-1);
        lua_pop(ls,2);
        lua_pushcfunction(ls, LuaEngine::recover_cfunction("InputEvent",arg) );
        return 1;
    });
    lua_settable(ls,-3);

    lua_setmetatable(ls,-2);

}
template<> Input::InputEvent* LuaEngine::pop(lua_State* ls ){
    Input::InputEvent** ret = static_cast<Input::InputEvent**>(lua_touserdata(ls,-1) );
    lua_pop(ls,1);
    return *ret;
}


bool         Input::key_pressed[GLFW_KEY_LAST+1];
bool         Input::mouse_pressed[ 3 ];
Vector2      Input::window_mouse_position;
std::list<Input::InputEvent*> Input::event_queue;    

Input::InputEvent* Input::pop_event_queue(){
    if( event_queue.size() > 0 ){
        InputEvent* ret = event_queue.front();
        event_queue.pop_front();
        return ret;
    } else return NULL;
}
void Input::mouse_pos_callback( GLFWwindow* glfw_window , double x_pos , double y_pos ){
    window_mouse_position = Vector2(x_pos,y_pos);
    Input::InputEvent* new_input_event = new Input::InputEvent();
    new_input_event->input_event_mouse_motion.is_solved = false;
    new_input_event->input_event_mouse_motion.type = INPUT_EVENT_TYPE_MOUSE_MOTION;
    new_input_event->input_event_mouse_motion.window_x = x_pos;
    new_input_event->input_event_mouse_motion.window_y = y_pos;
    // If last event queued is a mouse motion, it is discarded, since it hasn't been solved yet and we already got a new one
    if( !event_queue.empty() && event_queue.back()->input_event_mouse_motion.type == INPUT_EVENT_TYPE_MOUSE_MOTION ){
        delete event_queue.back();
        event_queue.pop_back();
    }
    event_queue.push_back( new_input_event );
}
void Input::key_callback( GLFWwindow* glfw_window , int key, int scancode, int action, int mods ){
    key_pressed[key] = (action!=RELEASED);
    Input::InputEvent* new_input_event = new Input::InputEvent();
    new_input_event->input_event_key.is_solved = false;
    new_input_event->input_event_key.type = INPUT_EVENT_TYPE_KEY;
    new_input_event->input_event_key.key_unicode = key;
    new_input_event->input_event_key.action = (INPUT_EVENT_ACTION)action;
    event_queue.push_back( new_input_event );
}
void Input::mouse_button_callback( GLFWwindow* glfw_window , int button , int action , int mods ){
    mouse_pressed[button] = (action!=RELEASED);
    Input::InputEvent* new_input_event = new Input::InputEvent();
    new_input_event->input_event_mouse_button.is_solved = false;
    new_input_event->input_event_mouse_button.type = INPUT_EVENT_TYPE_MOUSE_BUTTON;
    new_input_event->input_event_mouse_button.mouse_button = (INPUT_EVENT_MOUSE_BUTTON)button;
    new_input_event->input_event_mouse_button.action = (INPUT_EVENT_ACTION)action;
    event_queue.push_back( new_input_event );
}

void    Input::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( InputEvent , solve );
    REGISTER_LUA_MEMBER_FUNCTION( InputEvent , is_solved );
    REGISTER_LUA_MEMBER_FUNCTION( InputEvent , get_type_str );
    REGISTER_LUA_MEMBER_FUNCTION( InputEvent , get_mouse_position );
}

