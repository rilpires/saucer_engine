#ifndef COLLISION_H
#define COLLISION_H

#include <vector>

#include "component.h"
#include "box2d/box2d.h"

enum {
    SAUCER_BODY_TYPE_STATIC = b2_staticBody ,
    SAUCER_BODY_TYPE_KINEMATIC = b2_kinematicBody ,
    SAUCER_BODY_TYPE_DYNAMIC = b2_dynamicBody
};

class CollisionWorld : public SaucerObject {
    REGISTER_SAUCER_OBJECT(CollisionWorld , SaucerObject);

    class CollisionListener : public b2ContactListener {
        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);
    };

    private:
        int         velocity_iterations = 6;
        int         position_iterations = 2;
        b2World*    b2_world;
        Vector2     gravity;
        CollisionWorld::CollisionListener* listener;
    public:
        CollisionWorld();
        ~CollisionWorld();

        b2World*    get_b2_world();
        Vector2     get_gravity() const;
        void        set_gravity( Vector2 v );

        void        step();
        void        delete_disableds();
    
};

class CollisionBody : public Component {
    
    REGISTER_AS_COMPONENT(CollisionBody);

    private:

        std::vector< std::pair<b2FixtureDef,b2Shape*> >   fixture_defs;
        float                       restitution;
        float                       friction;
        float                       density;
        bool                        sensor;
        bool                        fixed_rotation;
        b2Body*                     b2_body;
        uint16_t                    collision_layer;
        uint16_t                    collision_mask;
        unsigned char               body_type;

    public:

        CollisionBody();
        ~CollisionBody();

        int                 get_body_type() const ;
        void                set_body_type( int type );
        void                create_rectangle_shape( Vector2 size , Vector2 offset );
        void                create_circle_shape( float radius , Vector2 offset );
        
        void                set_restitution( float new_val );
        float               get_restitution() const ;
        void                set_friction( float new_val );
        float               get_friction() const ;
        void                set_density( float new_val );
        float               get_density() const ;
        void                set_sensor( bool new_val );
        bool                get_sensor() const ;
        void                set_fixed_rotation( bool new_val );
        bool                get_fixed_rotation() const ;
        bool                get_collision_layer_bit( int bit );
        bool                get_collision_mask_bit( int bit );
        void                set_collision_layer_bit( int bit , bool new_val );
        void                set_collision_mask_bit( int bit , bool new_val );
        
        const std::vector<CollisionBody*> get_current_collisions() const ;

        // these below should not be exposed to scripting
        virtual void        collision_start( CollisionBody* other );
        virtual void        collision_end( CollisionBody* other );
        void                entered_tree();
        void                exiting_tree();
        Vector2             get_position() const;
        float               get_rotation_degrees() const;

        static void         bind_methods();
        void                push_editor_items();
        YamlNode            to_yaml_node() const override ;
        void                from_yaml_node( YamlNode ) override ;

};

#endif