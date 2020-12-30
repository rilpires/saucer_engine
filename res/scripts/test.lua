
counter = 0
original_pos = Vector2(0,100)

function frame_start( dt_seconds )
    original_pos = original_pos:rotated(5);
    this:set_position( original_pos )
    this:set_rotation_degrees( this:get_rotation_degrees() - 5 )
    
    res = this:get_image_texture()
    print( "res is " , res )
    print( "it's path is " , res:get_path() )

    if res:get_path() == "res/troll.png" then
        this:set_image_texture( ResourceManager.get_resource("res/gold.png") )
    else 
        this:set_image_texture( ResourceManager.get_resource("res/troll.png") )
    end

end
