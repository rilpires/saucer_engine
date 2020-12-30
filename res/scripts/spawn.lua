
delta_pos = Vector2(0,-1)


function frame_start( dt_seconds )
    this:set_position( this:get_position() + delta_pos )
    this:set_rotation_degrees( this:get_rotation_degrees() + 1 )
end
