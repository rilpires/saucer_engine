#ifndef SCENE_H
#define SCENE_H

#include "input.h"
#include "engine.h"
#include "transform.h"
#include "saucer_object.h"
#include "collision.h"
#include <vector>

class SceneNode;
class CameraNode;

class Scene : public SaucerObject {
    REGISTER_SAUCER_OBJECT(Scene,SaucerObject);

    friend class Window; // access current_window    
    private:

        CollisionWorld* collision_world;
        SceneNode*  root_node;
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
        CollisionWorld* get_collision_world() const ;
        void            loop();

        static void bind_methods();

};

#endif