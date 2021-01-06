#include "input.h"
#include "core.h"

// Push/pop definitions for Input::InputEvent* should be special, since it's the only type 
// that is passed by pointer and isn't a saucer object
template<> Input::InputEvent* LuaEngine::pop(lua_State* ls ){
    Input::InputEvent** ret = static_cast<Input::InputEvent**>(lua_touserdata(ls,-1) );
    lua_pop(ls,1);
    return *ret;
}

// push definition for InputEvent should be specialized since it is used in "LuaEngine::execute_input" here
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
        lua_pushcfunction(ls, LuaEngine::recover_nested_function("InputEvent",arg) );
        return 1;
    });
    lua_settable(ls,-3);
    lua_setmetatable(ls,-2);
}


bool         Input::key_pressed[GLFW_KEY_LAST+1];
bool         Input::mouse_pressed[ 3 ];
Vector2      Input::window_mouse_position;
std::list<Input::InputEvent*> Input::event_queue;    

Vector2      Input::get_world_mouse_position(){
    if( Engine::get_current_scene() ){
        Vector2 temp = window_mouse_position;
        temp.y = Engine::get_window_size().y - temp.y;
        temp -= Engine::get_window_size() * 0.5;
        return Engine::get_render_engine()->get_camera_transform() * temp;
    } else return window_mouse_position;
}
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
    REGISTER_LUA_MEMBER_FUNCTION( InputEvent , is_pressed );
    REGISTER_LUA_MEMBER_FUNCTION( InputEvent , is_echo );
    REGISTER_LUA_MEMBER_FUNCTION( InputEvent , get_type );
    REGISTER_LUA_MEMBER_FUNCTION( InputEvent , get_type_str );
    REGISTER_LUA_MEMBER_FUNCTION( InputEvent , get_mouse_position );

    REGISTER_LUA_NESTED_STATIC_FUNCTION( Input , is_key_pressed );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Input , is_mouse_button_pressed );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Input , get_screen_mouse_position );
    REGISTER_LUA_NESTED_STATIC_FUNCTION( Input , get_world_mouse_position );
    
    REGISTER_LUA_CONSTANT(InputEventType,KEY,INPUT_EVENT_TYPE_KEY);
    REGISTER_LUA_CONSTANT(InputEventType,MOUSE_BUTTON,INPUT_EVENT_TYPE_MOUSE_BUTTON);
    REGISTER_LUA_CONSTANT(InputEventType,MOUSE_MOTION,INPUT_EVENT_TYPE_MOUSE_MOTION);

    #define REGISTER_GLFW_KEY_INTO_LUA( k )\
        REGISTER_LUA_CONSTANT( KEY , k , GLFW_KEY_##k );
    // These were pretty much copied from GLFW '#define's ...
    REGISTER_GLFW_KEY_INTO_LUA(SPACE);
    REGISTER_GLFW_KEY_INTO_LUA(APOSTROPHE);
    REGISTER_GLFW_KEY_INTO_LUA(COMMA); 
    REGISTER_GLFW_KEY_INTO_LUA(MINUS); 
    REGISTER_GLFW_KEY_INTO_LUA(PERIOD); 
    REGISTER_GLFW_KEY_INTO_LUA(SLASH);
    REGISTER_GLFW_KEY_INTO_LUA(0);
    REGISTER_GLFW_KEY_INTO_LUA(1);
    REGISTER_GLFW_KEY_INTO_LUA(2);
    REGISTER_GLFW_KEY_INTO_LUA(3);
    REGISTER_GLFW_KEY_INTO_LUA(4);
    REGISTER_GLFW_KEY_INTO_LUA(5);
    REGISTER_GLFW_KEY_INTO_LUA(6);
    REGISTER_GLFW_KEY_INTO_LUA(7);
    REGISTER_GLFW_KEY_INTO_LUA(8);
    REGISTER_GLFW_KEY_INTO_LUA(9);
    REGISTER_GLFW_KEY_INTO_LUA(SEMICOLON);
    REGISTER_GLFW_KEY_INTO_LUA(EQUAL);
    REGISTER_GLFW_KEY_INTO_LUA(A);
    REGISTER_GLFW_KEY_INTO_LUA(B);
    REGISTER_GLFW_KEY_INTO_LUA(C);
    REGISTER_GLFW_KEY_INTO_LUA(D);
    REGISTER_GLFW_KEY_INTO_LUA(E);
    REGISTER_GLFW_KEY_INTO_LUA(F);
    REGISTER_GLFW_KEY_INTO_LUA(G);
    REGISTER_GLFW_KEY_INTO_LUA(H);
    REGISTER_GLFW_KEY_INTO_LUA(I);
    REGISTER_GLFW_KEY_INTO_LUA(J);
    REGISTER_GLFW_KEY_INTO_LUA(K);
    REGISTER_GLFW_KEY_INTO_LUA(L);
    REGISTER_GLFW_KEY_INTO_LUA(M);
    REGISTER_GLFW_KEY_INTO_LUA(N);
    REGISTER_GLFW_KEY_INTO_LUA(O);
    REGISTER_GLFW_KEY_INTO_LUA(P);
    REGISTER_GLFW_KEY_INTO_LUA(Q);
    REGISTER_GLFW_KEY_INTO_LUA(R);
    REGISTER_GLFW_KEY_INTO_LUA(S);
    REGISTER_GLFW_KEY_INTO_LUA(T);
    REGISTER_GLFW_KEY_INTO_LUA(U);
    REGISTER_GLFW_KEY_INTO_LUA(V);
    REGISTER_GLFW_KEY_INTO_LUA(W);
    REGISTER_GLFW_KEY_INTO_LUA(X);
    REGISTER_GLFW_KEY_INTO_LUA(Y);
    REGISTER_GLFW_KEY_INTO_LUA(Z);
    REGISTER_GLFW_KEY_INTO_LUA(LEFT_BRACKET);
    REGISTER_GLFW_KEY_INTO_LUA(BACKSLASH); 
    REGISTER_GLFW_KEY_INTO_LUA(RIGHT_BRACKET);
    REGISTER_GLFW_KEY_INTO_LUA(GRAVE_ACCENT);  
    REGISTER_GLFW_KEY_INTO_LUA(WORLD_1);
    REGISTER_GLFW_KEY_INTO_LUA(WORLD_2);
    REGISTER_GLFW_KEY_INTO_LUA(ESCAPE);
    REGISTER_GLFW_KEY_INTO_LUA(ENTER);
    REGISTER_GLFW_KEY_INTO_LUA(TAB);
    REGISTER_GLFW_KEY_INTO_LUA(BACKSPACE);
    REGISTER_GLFW_KEY_INTO_LUA(INSERT);
    REGISTER_GLFW_KEY_INTO_LUA(DELETE);
    REGISTER_GLFW_KEY_INTO_LUA(RIGHT);
    REGISTER_GLFW_KEY_INTO_LUA(LEFT);
    REGISTER_GLFW_KEY_INTO_LUA(DOWN);
    REGISTER_GLFW_KEY_INTO_LUA(UP);
    REGISTER_GLFW_KEY_INTO_LUA(PAGE_UP);
    REGISTER_GLFW_KEY_INTO_LUA(PAGE_DOWN);
    REGISTER_GLFW_KEY_INTO_LUA(HOME);
    REGISTER_GLFW_KEY_INTO_LUA(END);
    REGISTER_GLFW_KEY_INTO_LUA(CAPS_LOCK);
    REGISTER_GLFW_KEY_INTO_LUA(SCROLL_LOCK);
    REGISTER_GLFW_KEY_INTO_LUA(NUM_LOCK);
    REGISTER_GLFW_KEY_INTO_LUA(PRINT_SCREEN);
    REGISTER_GLFW_KEY_INTO_LUA(PAUSE);
    REGISTER_GLFW_KEY_INTO_LUA(F1);
    REGISTER_GLFW_KEY_INTO_LUA(F2);
    REGISTER_GLFW_KEY_INTO_LUA(F3);
    REGISTER_GLFW_KEY_INTO_LUA(F4);
    REGISTER_GLFW_KEY_INTO_LUA(F5);
    REGISTER_GLFW_KEY_INTO_LUA(F6);
    REGISTER_GLFW_KEY_INTO_LUA(F7);
    REGISTER_GLFW_KEY_INTO_LUA(F8);
    REGISTER_GLFW_KEY_INTO_LUA(F9);
    REGISTER_GLFW_KEY_INTO_LUA(F10);
    REGISTER_GLFW_KEY_INTO_LUA(F11);
    REGISTER_GLFW_KEY_INTO_LUA(F12);
    REGISTER_GLFW_KEY_INTO_LUA(F13);
    REGISTER_GLFW_KEY_INTO_LUA(F14);
    REGISTER_GLFW_KEY_INTO_LUA(F15);
    REGISTER_GLFW_KEY_INTO_LUA(F16);
    REGISTER_GLFW_KEY_INTO_LUA(F17);
    REGISTER_GLFW_KEY_INTO_LUA(F18);
    REGISTER_GLFW_KEY_INTO_LUA(F19);
    REGISTER_GLFW_KEY_INTO_LUA(F20);
    REGISTER_GLFW_KEY_INTO_LUA(F21);
    REGISTER_GLFW_KEY_INTO_LUA(F22);
    REGISTER_GLFW_KEY_INTO_LUA(F23);
    REGISTER_GLFW_KEY_INTO_LUA(F24);
    REGISTER_GLFW_KEY_INTO_LUA(F25);
    REGISTER_GLFW_KEY_INTO_LUA(KP_0);
    REGISTER_GLFW_KEY_INTO_LUA(KP_1);
    REGISTER_GLFW_KEY_INTO_LUA(KP_2);
    REGISTER_GLFW_KEY_INTO_LUA(KP_3);
    REGISTER_GLFW_KEY_INTO_LUA(KP_4);
    REGISTER_GLFW_KEY_INTO_LUA(KP_5);
    REGISTER_GLFW_KEY_INTO_LUA(KP_6);
    REGISTER_GLFW_KEY_INTO_LUA(KP_7);
    REGISTER_GLFW_KEY_INTO_LUA(KP_8);
    REGISTER_GLFW_KEY_INTO_LUA(KP_9);
    REGISTER_GLFW_KEY_INTO_LUA(KP_DECIMAL);
    REGISTER_GLFW_KEY_INTO_LUA(KP_DIVIDE);
    REGISTER_GLFW_KEY_INTO_LUA(KP_MULTIPLY);
    REGISTER_GLFW_KEY_INTO_LUA(KP_SUBTRACT);
    REGISTER_GLFW_KEY_INTO_LUA(KP_ADD);
    REGISTER_GLFW_KEY_INTO_LUA(KP_ENTER);
    REGISTER_GLFW_KEY_INTO_LUA(KP_EQUAL);
    REGISTER_GLFW_KEY_INTO_LUA(LEFT_SHIFT);
    REGISTER_GLFW_KEY_INTO_LUA(LEFT_CONTROL);
    REGISTER_GLFW_KEY_INTO_LUA(LEFT_ALT);
    REGISTER_GLFW_KEY_INTO_LUA(LEFT_SUPER);
    REGISTER_GLFW_KEY_INTO_LUA(RIGHT_SHIFT);
    REGISTER_GLFW_KEY_INTO_LUA(RIGHT_CONTROL);
    REGISTER_GLFW_KEY_INTO_LUA(RIGHT_ALT);
    REGISTER_GLFW_KEY_INTO_LUA(RIGHT_SUPER);
    REGISTER_GLFW_KEY_INTO_LUA(MENU);


}

