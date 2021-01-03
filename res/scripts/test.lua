
function _entered_tree()

    window_size = Engine.get_window_size()
    wall = SceneNode.new()
    wall_body = wall:create_body()
    wall_body:set_body_type( BodyType.STATIC )
    for index,center in { Vector2(-window_size.x,0) , Vector2(window_size.x,0) , Vector2(0,-window_size.y) } do
        wall_body:create_rectangle_shape( window_size , center )
    end
    this:add_child(wall)
    
end

function _frame_start( dt_seconds )
    -- vel = Vector2(0,0);
    -- if (Input.is_key_pressed(KEY.LEFT))     then vel.x = vel.x - 1.0 end;
    -- if (Input.is_key_pressed(KEY.RIGHT))    then vel.x = vel.x + 1.0 end;
    -- if (Input.is_key_pressed(KEY.UP))       then vel.y = vel.y + 1.0 end;
    -- if (Input.is_key_pressed(KEY.DOWN))     then vel.y = vel.y - 1.0 end;
    -- this:set_position( this:get_position() + vel*5.0 )
    print("FPS: " , Engine.get_fps() )
end


function _input( input_event )
    if( input_event:get_type() == InputEventType.MOUSE_BUTTON and input_event:is_pressed() and not input_event:is_echo() ) then
        new_spawn = SceneNode.new()
        this:add_child(new_spawn)

        new_spawn:set_global_position( Input.get_world_mouse_position() )
        new_spawn:create_sprite()
        new_spawn:get_sprite():set_texture( load("res/gold.png") )
        new_spawn:create_body()
        new_spawn:get_body():set_body_type( BodyType.DYNAMIC )
        new_spawn:get_body():create_rectangle_shape( Vector2(25,25)*0.9 , Vector2(0,0) )

    end
end