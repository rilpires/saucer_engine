
function frame_start( dt_seconds )

    for i=1,2 do
        new_child = SceneNode.new()
        new_child:set_position( Vector2( math.random(-100,100) , math.random(-100,100) ) )
        new_child:set_image_texture( ResourceManager.get_resource("res/gold.png") )
        new_child:set_script_resource( ResourceManager.get_resource("res/scripts/spawn.lua") )
        this:add_child( new_child )
    end

    children = this:get_children()

    print("I have " , table.getn( children ) , " children ")
    print("FPS: " , Engine.get_fps() )

end
