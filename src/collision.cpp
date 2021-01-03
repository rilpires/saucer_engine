#include "collision.h"
#include "core.h"
#include "math.h"

#define METERS_PER_PIXEL 0.01f

CollisionWorld::CollisionWorld(){
    gravity = Vector2( 1.0f , -10.0f ) ;
    b2_world = new b2World( b2Vec2(gravity.x , gravity.y ) );
}
CollisionWorld::~CollisionWorld(){
    delete b2_world;
}

b2World*    CollisionWorld::get_b2_world(){
    return b2_world;
}
Vector2     CollisionWorld::get_gravity() const{
    return gravity;
}
void        CollisionWorld::set_gravity( Vector2 v ){
    gravity = v;
    b2_world->SetGravity( b2Vec2(gravity.x * METERS_PER_PIXEL , gravity.y * METERS_PER_PIXEL ) );
}

void        CollisionWorld::step(){
    b2_world->Step( 1.0f / 60.0f , velocity_iterations , position_iterations );
}


// ---------------------------------------------------------------------------


std::unordered_map< SaucerId , CollisionBody* > CollisionBody::component_from_node;

CollisionBody::CollisionBody(){
    b2_body = nullptr;
    restitution = 0.5;
    friction = 0.5;
    density = 1.0;
    sensor = false;
    body_type = SAUCER_BODY_TYPE_STATIC;
}
CollisionBody::~CollisionBody(){
    if(b2_body){
        b2World* world = get_node()->get_scene()->get_collision_world()->get_b2_world();
        world->DestroyBody( b2_body );
    }
}

int     CollisionBody::get_body_type() const {
    return body_type;
}
void    CollisionBody::set_body_type( int new_type ){
    if( new_type != body_type ){
        body_type = new_type;
        if( b2_body ){
            b2_body->SetType((b2BodyType)new_type);
        }
    }
}

void    CollisionBody::create_rectangle_shape( Vector2 size , Vector2 offset ){
    b2PolygonShape* shape = new b2PolygonShape();
    shape->SetAsBox( size.x * 0.5 * METERS_PER_PIXEL , size.y * 0.5 * METERS_PER_PIXEL );
    for(int i = 0;i<4;i++ ){
        shape->m_vertices[i].x += offset.x * METERS_PER_PIXEL;
        shape->m_vertices[i].y += offset.y * METERS_PER_PIXEL;
    }
    b2FixtureDef def;
    def.shape = shape;
    def.density = density;
    def.restitution = restitution;
    def.friction = friction;
    def.isSensor = sensor;
    def.userData.pointer = (uintptr_t) get_node();
    fixture_defs.push_back( std::make_pair(def,shape) );
    if( b2_body ) b2_body->CreateFixture( &def );
}
void    CollisionBody::create_circle_shape( float radius , Vector2 offset ){
    b2CircleShape* shape = new b2CircleShape();
    shape->m_radius = radius * METERS_PER_PIXEL;
    shape->m_p.x += offset.x * METERS_PER_PIXEL;
    shape->m_p.y += offset.y * METERS_PER_PIXEL;
    b2FixtureDef def;
    def.shape = shape;
    def.density = density;
    def.restitution = restitution;
    def.friction = friction;
    def.isSensor = sensor;
    def.userData.pointer = (uintptr_t) get_node();
    fixture_defs.push_back( std::make_pair(def , shape) );
    if( b2_body ) b2_body->CreateFixture( &def );
}

void    CollisionBody::set_restitution( float new_val ){
    restitution = new_val;
}
float   CollisionBody::get_restitution() const {
    return restitution;
}
void    CollisionBody::set_friction( float new_val ){
    friction = new_val;
}
float   CollisionBody::get_friction() const {
    return friction;
}
void    CollisionBody::set_density( float new_val ){
    density = new_val;
}
float   CollisionBody::get_density() const {
    return density;
}
void    CollisionBody::set_sensor( bool new_val ){
    sensor = new_val;
}
bool    CollisionBody::is_sensor() const {
    return sensor;
}


void    CollisionBody::tree_changed(){

    bool is_current_in_scene = get_node()->get_scene();
    
    if( !is_current_in_scene && b2_body ){
        // Gotta delete this body
        b2_body->GetWorld()->DestroyBody( b2_body );
    }
    else if (is_current_in_scene && !b2_body ){
        // Gotta create this body
        SceneNode* node = get_node();
        b2World* world = node->get_scene()->get_collision_world()->get_b2_world();
        Vector2 node_global_pos = node->get_global_position();
        
        b2BodyDef def;
        def.position = b2Vec2( node_global_pos.x * METERS_PER_PIXEL , node_global_pos.y * METERS_PER_PIXEL );
        def.angle = node->get_global_rotation_degrees();
        def.type = (b2BodyType)body_type;
        
        b2_body = world->CreateBody( &def );
        for( auto& p : fixture_defs )
            b2_body->CreateFixture( &(p.first) );
    }
}
Vector2 CollisionBody::get_position() const{
    b2Vec2 v = b2_body->GetPosition();
    Vector2 ret = Vector2(v.x,v.y) / METERS_PER_PIXEL;
    return ret;
}
float   CollisionBody::get_rotation_degrees() const{
    return b2_body->GetAngle() * 180.0f / M_PI ;
}

void    CollisionBody::bind_methods(){
    REGISTER_LUA_CONSTANT( BodyType , DYNAMIC , SAUCER_BODY_TYPE_DYNAMIC );
    REGISTER_LUA_CONSTANT( BodyType , STATIC , SAUCER_BODY_TYPE_STATIC );
    REGISTER_LUA_CONSTANT( BodyType , KINEMATIC , SAUCER_BODY_TYPE_KINEMATIC );

    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , get_body_type );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , set_body_type );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , create_rectangle_shape );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , create_circle_shape );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , set_restitution );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , get_restitution );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , set_friction );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , get_friction );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , set_density );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , get_density );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , set_sensor );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , is_sensor );

}