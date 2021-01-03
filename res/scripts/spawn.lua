
function _frame_start( dt_seconds )
    if not this:get_body() then
        this:set_rotation_degrees( this:get_rotation_degrees() + 3 )
    else
        current_collisions = this:get_body():get_current_collisions();
        if table.getn(current_collisions) > 0 then
            print("current collisions: " )
            for k,v in current_collisions do
                print( k , " ; " , v , " ; " , v:get_node() )
            end     
        end
    end
end

function _collision_start( other )
    this:get_out()
end