
function _entered_tree()
    this:set_modulate(Color(0.5,1,1,1))
    window_size = Engine.get_window_size()
    wall = SceneNode.new()
    wall_body = wall:create_body()
    wall_body:set_body_type( BodyType.STATIC )
    for index,center in { Vector2(-window_size.x,0) , Vector2(window_size.x,0) , Vector2(0,window_size.y) } do
        wall_body:create_rectangle_shape( window_size , center )
    end
    this:add_child(wall)
    this:create_camera():set_active(true)
    this:get_camera():set_zoom(Vector2(1,1))
end

function _frame_start( dt_seconds )
    if not this:get_sprite() then
        this:create_sprite():set_texture( load("res/gold.png") )
    end
    vel = Vector2(0,0);
    if (Input.is_key_pressed(KEY.LEFT))     then vel.x = vel.x - 1.0 end;
    if (Input.is_key_pressed(KEY.RIGHT))    then vel.x = vel.x + 1.0 end;
    if (Input.is_key_pressed(KEY.UP))       then vel.y = vel.y - 1.0 end;
    if (Input.is_key_pressed(KEY.DOWN))     then vel.y = vel.y + 1.0 end;
    this:set_global_position( this:get_global_position() + vel*5.0 )
    
    print("FPS: " , Engine.get_fps() )
end

current_spawn = nil
h_frames = 4;
v_frames = 3;

function _input( input_event )
    if( input_event:get_type() == InputEventType.MOUSE_BUTTON and input_event:is_pressed() ) then
        this.current_spawn = SceneNode.new()
        this.current_spawn:set_script( load("res/scripts/spawn.lua") )
        this:add_child(this.current_spawn)
        this.current_spawn:set_global_position( Input.get_world_mouse_position() )
        this.current_spawn:create_sprite()
        this.current_spawn:get_sprite():set_texture( load("res/troll.png") ) 
        this.current_spawn:get_sprite():set_h_frames(this.h_frames) 
        this.current_spawn:get_sprite():set_v_frames(this.v_frames) 
        this.current_spawn:set_self_modulate(Color(1,1,1,0.5))
        this.current_spawn:get_sprite():set_frame_index(8)
    elseif ( input_event:get_type() == InputEventType.MOUSE_BUTTON and not input_event:is_pressed() ) then 
        local body = this.current_spawn:create_body()
        local sprite = this.current_spawn:get_sprite()
        this.current_spawn:set_scale(Vector2(1,1))
        this.current_spawn:set_rotation_degrees(0)
        body:set_body_type( BodyType.DYNAMIC )
        body:set_restitution( 0.15 )
        body:set_sensor( false )
        body:set_fixed_rotation( math.random(0,1)>0.5 )
        body:create_rectangle_shape( sprite:get_texture():get_size()/Vector2(this.h_frames,this.v_frames) , Vector2(0,0) )
        this.current_spawn = nil
    elseif ( input_event:get_type() == InputEventType.MOUSE_MOTION and this.current_spawn ) then
        this.current_spawn:set_global_position( Input.get_world_mouse_position() )
    end

end