velocity = 0

function frame_start( dt_seconds )
    print("my get_position().x is " , get_position().x )
    print("my get_parent().get_position().x is " , get_parent():get_position().x )
    
    -- Script is kinda verbose, need to add some Vector2 operators

    pos = get_position()
    if velocity==0 then
        velocity = get_position()
        velocity.x = 0
        velocity.y = 0
    end
    if pos.x>0 then
        velocity.x = velocity.x - math.abs(pos.x) * 0.05
    else
        velocity.x = velocity.x + math.abs(pos.x) * 0.05
    end
    if pos.y>0 then
        velocity.y = velocity.y - math.abs(pos.y) * 0.03
    else
        velocity.y = velocity.y + math.abs(pos.y) * 0.03
    end
    
    pos = get_position()
    pos.x = pos.x + velocity.x;
    pos.y = pos.y + velocity.y;
    
    set_position( pos )
end

