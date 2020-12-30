#ifndef SCENE_H
#define SCENE_H

#include "input.h"
#include "engine.h"
#include "transform.h"
#include "saucer_object.h"

#include <vector>

class SceneNode;
class CameraNode;

class Scene : public SaucerObject {
    
    friend class Window; // access current_window

    private:

        SceneNode*  root_node;
        Transform   camera_transform;
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
        
        static Scene*   lua_new();

        void            set_root_node(SceneNode* p_root_node);
        SceneNode*      get_root_node();

        Transform       get_camera_transform();
        void            set_camera_transform(Transform t);

        void            propagate_input_event( Input::InputEvent* input_event );
        void            loop();

        static void bind_methods();

};

#endif