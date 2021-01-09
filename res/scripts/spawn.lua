
function _entered_tree()
    patch = this:create_patch_rect(); 
    patch:set_rect_pos(  Vector2(-50,0) )
    patch:set_rect_size( Vector2(100,100) )
    patch:set_texture( load("res/border.png") )
    patch:set_use_scene_node_transform(false)

    new_node = SceneNode.new()
    this:add_child(new_node)
    child_rect = new_node:create_patch_rect()
    child_rect:set_anchored_border(0,false)
    child_rect:set_anchored_border(1,true)
    child_rect:set_anchored_border(2,false)
    child_rect:set_anchored_border(3,false)
    child_rect:set_rect_pos(Vector2(-50,50))
    child_rect:set_rect_size(Vector2(70,70))
    child_rect:set_starts_on_viewport(false)
    child_rect:set_texture( load("res/border.png") )
    -- this:get_parent():add_child(new_node)
end

function _frame_start( dt_seconds )
    local id = this:get_saucer_id()
    local r = math.sin( id*123 );
    t = Engine.get_uptime()
    this:set_modulate( Color(r,1,1,1) )
    if not this:get_body() then
        local s = math.sin( Engine.get_uptime() * 200.0 + id*13 );
        this:set_rotation_degrees( this:get_rotation_degrees() + 3 )
        -- this:set_scale( Vector2( s , 1 ) )
    end
    patch = this:get_patch_rect()
    patch:set_rect_size( Vector2( 160 + 22*math.cos( t * 110 ) , 160 + 20*math.sin( t * 30 ) ) )
end

list_of_sounds = {
    load("res/sounds/boom.wav")
}



function _collision_start( other )
    local audio_emitter = this:get_audio_emitter()
    if not audio_emitter then
        audio_emitter = this:create_audio_emitter()
        i = math.random(1,table.getn(list_of_sounds))
        audio_emitter:set_audio_resource( list_of_sounds[i] )
    end
    -- audio_emitter:play()
    sprite = this:get_sprite()
    sprite:set_frame_index( sprite:get_frame_index()+1 )
    -- this:get_parent():get_out()
end