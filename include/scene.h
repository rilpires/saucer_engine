#ifndef SCENE_H
#define SCENE_H

#include "input.h"
#include "window.h"
#include <vector>
#include "saucer_object.h"

class SceneNode;
class CameraNode;

class Scene : public SaucerObject {
    
    friend class Window; // access current_window

    private:

        SceneNode*  root_node;
        CameraNode* current_camera;
        Window*     current_window;

        std::vector<SceneNode*> current_draws;
        std::vector<SceneNode*> current_input_handlers;
        std::vector<SceneNode*> current_script_actors;
        std::vector<SceneNode*> current_physics_actors;

        void        update_current_actors();
        void        loop_draw();
        void        loop_input();
        void        loop_script();
        void        loop_physics();

    public:
        Scene();
        ~Scene();
        
        void        set_root_node(SceneNode* p_root_node);
        SceneNode*  get_root_node(){return root_node;};

        CameraNode* get_current_camera(){return current_camera;}
        void        set_current_camera(CameraNode* new_cam){current_camera=new_cam;}

        Window*     get_current_window(){return current_window;}

        void        propagate_input_event( Input::InputEvent* input_event );
        void        loop();

};

#endif