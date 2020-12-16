#ifndef SCENE_H
#define SCENE_H

#include "input.h"

class SceneNode;
class CameraNode;

class Scene {
    private:

        SceneNode*  root_node;
        CameraNode* current_camera;

    public:
        Scene();
        ~Scene();
        
        void        set_root_node(SceneNode* p_root_node);
        SceneNode*  get_root_node(){return root_node;};

        CameraNode* get_current_camera(){return current_camera;}
        void        set_current_camera(CameraNode* new_cam){current_camera=new_cam;}

        void        propagate_input_event( Input::InputEvent* input_event );

};

#endif