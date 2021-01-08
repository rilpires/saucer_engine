
function _frame_start( dt_seconds )
    local id = this:get_saucer_id()
    local r = math.sin( id*123 );
    this:set_modulate( Color(r,1,1,1) )
    if not this:get_body() then
        local s = math.sin( Engine.get_uptime() * 200.0 + id*13 );
        this:set_rotation_degrees( this:get_rotation_degrees() + 3 )
        this:set_scale( Vector2( s , 1 ) )
    end
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
    audio_emitter:play()
    sprite = this:get_sprite()
    sprite:set_frame_index( sprite:get_frame_index()+1 )
    -- this:get_parent():get_out()
end