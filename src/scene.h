#ifndef SCENE_H
#define SCENE_H

#include "input.h"
#include "engine.h"
#include "transform.h"
#include "saucer_object.h"
#include "collision.h"
#include <set>
#include <vector>

class AnchoredRect;
class SceneNode;
class Camera;

class Scene : public SaucerObject {
    REGISTER_SAUCER_OBJECT(Scene,SaucerObject);

    friend class Window; // access current_window    
    friend class AnchoredRect; // To nullify current_hovered_anchored_rect if needed

    private:
        CollisionWorld* collision_world;
        SceneNode*      root_node;
        Camera*         current_camera;   
        AnchoredRect*   current_hovered_anchored_rect;
        AnchoredRect*   current_focused_anchored_rect;
        std::set<SceneNode*> to_free;

        void        loop_input();
        void        loop_draw();
        void        loop_audio();
        void        loop_script();
        void        loop_physics();

        void        set_current_hovered_anchored_rect( AnchoredRect* r );
        void        set_current_focused_anchored_rect( AnchoredRect* r );

    public:
        Scene();
        ~Scene();
        
        static Scene*   lua_new();

        void            set_root_node(SceneNode* p_root_node);
        SceneNode*      get_root_node();
        void            set_current_camera( Camera* new_camera );
        AnchoredRect*   get_current_hovered_anchored_rect() const;
        AnchoredRect*   get_current_focused_anchored_rect() const;
        Camera*         get_current_camera() const ;
        CollisionWorld* get_collision_world() const ;

        void            queue_free_node(SceneNode* node);
        void            loop();

        static void bind_methods();

    private:
        struct AccumulatedTreeNode{
            Transform t;
            Color c;
            SceneNode* n;
        };

};

#endif