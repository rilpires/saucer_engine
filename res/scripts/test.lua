velocity = 0

function frame_start( dt_seconds )
    pos = this:get_position()
    pos.y = pos.y + 0.5 ;
    this:set_position(pos);
    this:set_rotation_degrees( this:get_rotation_degrees() - 5 )
end
