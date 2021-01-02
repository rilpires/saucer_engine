

function _frame_start( dt_seconds )
    vel = Vector2(0,0);
    
    if (Input.is_key_pressed(KEY.LEFT))     then vel.x = vel.x - 1.0 end;
    if (Input.is_key_pressed(KEY.RIGHT))    then vel.x = vel.x + 1.0 end;
    if (Input.is_key_pressed(KEY.UP))       then vel.y = vel.y + 1.0 end;
    if (Input.is_key_pressed(KEY.DOWN))     then vel.y = vel.y - 1.0 end;

    this:set_position( this:get_position() + vel*5.0 )
end


function _input( input_event )
    if( input_event:get_type_str() == "key" ) then
        this:get_sprite():set_texture( ResourceManager.get_resource("res/troll.png") )
    end
end