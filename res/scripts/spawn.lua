
function _frame_start( dt_seconds )
    -- if not this:get_body() then
    --     this:set_rotation_degrees( this:get_rotation_degrees() + 3 )
    -- else
    --     current_collisions = this:get_body():get_current_collisions();
    --     if table.getn(current_collisions) > 0 then
    --         print("current collisions: " )
    --         for k,v in current_collisions do
    --             print( k , " ; " , v , " ; " , v:get_node() )
    --         end     
    --     end
    -- end
end

list_of_sounds = {
    load("res/sounds/beam-out.wav") ,
    load("res/sounds/boom.wav") ,
    load("res/sounds/cursor.wav") ,
    load("res/sounds/dead.wav") ,
    load("res/sounds/dink.wav") ,
    load("res/sounds/enemy-hit.wav") ,
    load("res/sounds/hurt.wav") ,
    load("res/sounds/land.wav") ,
    load("res/sounds/one-up.wav") ,
    load("res/sounds/pause-close.wav") ,
    load("res/sounds/pause-open.wav") ,
    load("res/sounds/refill.wav") ,
    load("res/sounds/shot.wav") ,
    load("res/sounds/shutter.wav") ,
    load("res/sounds/weapon-get.wav") ,
    load("res/sounds/wily-boom.wav") ,
    load("res/sounds/wily-crash.wav") ,
    load("res/sounds/yoku.wav") ,
}



function _collision_start( other )
    local audio_emitter = this:get_audio_emitter()
    if not audio_emitter then
        audio_emitter = this:create_audio_emitter()
        i = math.random(1,table.getn(list_of_sounds))
        audio_emitter:set_audio_resource( list_of_sounds[i] )
    end
    audio_emitter:play()
end