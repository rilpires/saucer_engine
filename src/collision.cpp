#include "collision.h"
#include "lua_engine.h"
#include "scene_node.h"
#include "scene.h"
#include "math.h"

#define METERS_PER_PIXEL 0.01f

void CollisionWorld::CollisionListener::BeginContact(b2Contact* contact){
    CollisionBody* body_a = (CollisionBody*) contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    CollisionBody* body_b = (CollisionBody*) contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    if( body_a && body_b ){
        body_a->collision_start( body_b );
        body_b->collision_start( body_a );
    }
}
void CollisionWorld::CollisionListener::EndContact(b2Contact* contact){
    CollisionBody* body_a = (CollisionBody*) contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    CollisionBody* body_b = (CollisionBody*) contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    if( body_a && body_b ){
        body_a->collision_end( body_b );
        body_b->collision_end( body_a );
    }
}

CollisionWorld::CollisionWorld(){
    gravity = Vector2( 0.0f , 10.0f ) ;
    b2_world = new b2World( b2Vec2(gravity.x , gravity.y ) );
    listener = new CollisionWorld::CollisionListener();
    b2_world->SetContactListener( listener );
}
CollisionWorld::~CollisionWorld(){
    delete listener;
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
void        CollisionWorld::delete_disableds(){
    // I'm not sure if I should iterate over bodies that I'll be destroying so I'll use a vector just for sure
    std::vector<b2Body*> to_delete;
    for( b2Body* body = b2_world->GetBodyList() ; body ; body = body->GetNext() )
        if( body->GetUserData().pointer == 0 ) 
            to_delete.push_back(body);
    for( b2Body* body : to_delete )
        b2_world->DestroyBody(body);
}

// ---------------------------------------------------------------------------


std::unordered_multimap< SaucerId , CollisionBody* > CollisionBody::component_from_node;

CollisionBody::CollisionBody(){
    b2_body = nullptr;
    restitution = 0.5;
    friction = 0.1;
    density = 1.0;
    sensor = false;
    fixed_rotation = true;
    body_type = SAUCER_BODY_TYPE_STATIC;
    collision_layer = 0x0001;
    collision_mask  = 0xFFFF;
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
    def.filter.maskBits = collision_mask;
    def.filter.categoryBits = collision_layer;
    fixture_defs.push_back( std::make_pair(def , shape) );
    if( b2_body ) b2_body->CreateFixture( &def );
}

void    CollisionBody::set_restitution( float new_val ){
    restitution = new_val;
    for( auto& p : fixture_defs ) p.first.restitution = restitution;
    if(b2_body) for( b2Fixture* fixture = b2_body->GetFixtureList() ; fixture ; fixture = fixture->GetNext() ){
        fixture->SetRestitution( restitution );
    }
}
float   CollisionBody::get_restitution() const {
    return restitution;
}
void    CollisionBody::set_friction( float new_val ){
    friction = new_val;
    for( auto& p : fixture_defs ) p.first.friction = friction;
    if(b2_body) for( b2Fixture* fixture = b2_body->GetFixtureList() ; fixture ; fixture = fixture->GetNext() ){
        fixture->SetFriction( friction );
    }
}
float   CollisionBody::get_friction() const {
    return friction;
}
void    CollisionBody::set_density( float new_val ){
    density = new_val;
    for( auto& p : fixture_defs ) p.first.density = density;
    if(b2_body) for( b2Fixture* fixture = b2_body->GetFixtureList() ; fixture ; fixture = fixture->GetNext() ){
        fixture->SetDensity( density );
    }
}
float   CollisionBody::get_density() const {
    return density;
}
void    CollisionBody::set_sensor( bool new_val ){
    sensor = new_val;
    for( auto& p : fixture_defs ) p.first.isSensor = sensor;
    if(b2_body) for( b2Fixture* fixture = b2_body->GetFixtureList() ; fixture ; fixture = fixture->GetNext() ){
        fixture->SetSensor( sensor );
    }
}
bool    CollisionBody::is_sensor() const {
    return sensor;
}
void    CollisionBody::set_fixed_rotation( bool new_val ){
    fixed_rotation = new_val;
    if( b2_body ) {
        b2_body->SetFixedRotation( fixed_rotation );
    }
}
bool    CollisionBody::has_fixed_rotation() const {
    return fixed_rotation;
}
bool    CollisionBody::get_collision_layer_bit( int bit ){
    if( bit < 0 || bit > 16 ){
        saucer_err( "get_collision_layer_bit  only accepts values for \"bit\" such that 1 <= bit <= 16 " )
        return false;
    }
    return collision_layer & (1 << (bit-1) ); 
}
bool    CollisionBody::get_collision_mask_bit( int bit ){
    if( bit < 0 || bit > 16 ){
        saucer_err( "get_collision_mask_bit  only accepts values for \"bit\" such that 1 <= bit <= 16 " )
        return false;
    }
    return collision_mask & (1 << (bit-1) ); 
}
void    CollisionBody::set_collision_layer_bit( int bit , bool new_val ){
    if( bit < 0 || bit > 16 ){
        saucer_err( "set_collision_layer_bit only accepts values for \"bit\" such that 1 <= bit <= 16 " )
    } else 
    collision_layer = (new_val) ?   ( collision_layer |  (1<<(bit-1))) 
                                :   ( collision_layer & ~(1<<(bit-1)));

    for( auto& p : fixture_defs ) p.first.filter.categoryBits = collision_layer;
    if(b2_body) for( b2Fixture* fixture = b2_body->GetFixtureList() ; fixture ; fixture = fixture->GetNext() ){
        b2Filter new_filter = fixture->GetFilterData();
        new_filter.categoryBits = collision_layer;
        fixture->SetFilterData( new_filter );
    }
}
void    CollisionBody::set_collision_mask_bit( int bit , bool new_val ){
    if( bit < 0 || bit > 16 ){
        saucer_err( "set_collision_mask_bit only accepts values for \"bit\" such that 1 <= bit <= 16 " )
    } else
    collision_mask = (new_val) ?    ( collision_mask |  (1<<(bit-1))) 
                                :   ( collision_mask & ~(1<<(bit-1)));

    for( auto& p : fixture_defs ) p.first.filter.categoryBits = collision_mask;
    if(b2_body) for( b2Fixture* fixture = b2_body->GetFixtureList() ; fixture ; fixture = fixture->GetNext() ){
        b2Filter new_filter = fixture->GetFilterData();
        new_filter.categoryBits = collision_mask;
        fixture->SetFilterData( new_filter );
    }
}
const   std::vector<CollisionBody*> CollisionBody::get_current_collisions() const {
    std::vector<CollisionBody*> ret;
    if( b2_body ) for( const b2ContactEdge* edge = b2_body->GetContactList() ; edge ; edge = edge->next ){
        if( edge->contact->IsTouching() ){
            CollisionBody* body_a = (CollisionBody*)(edge->contact->GetFixtureA()->GetBody()->GetUserData().pointer);
            CollisionBody* body_b = (CollisionBody*)(edge->contact->GetFixtureB()->GetBody()->GetUserData().pointer);
            if( body_a && body_b ) ret.push_back( (this==body_a)?(body_b):(body_a) );
        }
    }
    return ret;
}



void    CollisionBody::collision_start( CollisionBody* other ){
    LuaEngine::execute_callback( "collision_start" , get_node() , other->get_node() );
}
void    CollisionBody::collision_end( CollisionBody* other ){
    LuaEngine::execute_callback( "collision_end" , get_node() , other->get_node() );
}
void    CollisionBody::entered_tree(){
    SceneNode* node = get_node();
    if( !b2_body ){
    // Gotta create this body
        b2World* world = node->get_scene()->get_collision_world()->get_b2_world();
        Vector2 node_global_pos = node->get_global_position();
        node->set_inherits_transform(false);
        node->set_position(node_global_pos);
        float node_global_rot_radians = node->get_global_rotation_degrees() * M_PI / 180.0 ;    
        b2BodyDef def;
        def.position = b2Vec2( node_global_pos.x * METERS_PER_PIXEL , node_global_pos.y * METERS_PER_PIXEL );
        def.angle = node_global_rot_radians;
        def.type = (b2BodyType)body_type;
        def.fixedRotation = fixed_rotation;
        def.userData.pointer = (uintptr_t) this;
        
        b2_body = world->CreateBody( &def );
        for( auto& p : fixture_defs ) b2_body->CreateFixture( &(p.first) );
    }
}
void    CollisionBody::exiting_tree(){
    if( b2_body ){
        // Gotta delete this body... but this tree_changed can be occurring inside a CollisionListener call...
        // So we should only nullify b2_body here and disable the body.
        // In the end of Scene::loop_physics we iterate over all bodies to search for disabled bodies and delete them
        get_node()->set_inherits_transform(true);
        b2_body->GetUserData().pointer = 0 ;
        b2_body = nullptr;
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


    REGISTER_LUA_CONSTANT( BodyType , DYNAMIC   , SAUCER_BODY_TYPE_DYNAMIC );
    REGISTER_LUA_CONSTANT( BodyType , STATIC    , SAUCER_BODY_TYPE_STATIC );
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
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , set_fixed_rotation );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , has_fixed_rotation );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , get_current_collisions );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , get_collision_layer_bit );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , get_collision_mask_bit );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , set_collision_layer_bit );
    REGISTER_LUA_MEMBER_FUNCTION( CollisionBody , set_collision_mask_bit );

}
YamlNode        CollisionBody::to_yaml_node() const {
    YamlNode ret;
    ret["restitution"] = restitution;
    ret["friction"] = friction;
    ret["density"] = density;
    ret["sensor"] = sensor;
    ret["fixed_rotation"] = fixed_rotation;
    ret["collision_layer"] = collision_layer;
    ret["collision_mask"] = collision_mask;
    ret["body_type"] = (int)body_type;
    return ret;
}
void            CollisionBody::from_yaml_node( YamlNode yaml_node ){
    set_restitution( yaml_node["restitution"].as<decltype(restitution)>() );
    set_friction( yaml_node["friction"].as<decltype(friction)>() );
    set_density( yaml_node["density"].as<decltype(density)>() );
    set_sensor( yaml_node["sensor"].as<decltype(sensor)>() );
    set_fixed_rotation( yaml_node["fixed_rotation"].as<decltype(fixed_rotation)>() );
    collision_layer = yaml_node["collision_layer"].as<decltype(collision_layer)>();
    collision_layer = yaml_node["collision_mask"].as<decltype(collision_mask)>();
    set_body_type( yaml_node["body_type"].as<decltype(body_type)>() );
}