label_rect = 0

function _entered_tree()
    
    patch_rect = this:create_patch_rect()
    patch_rect:set_rect_size( Vector2(100,100))
    patch_rect:set_use_scene_node_transform(true)
    patch_rect:set_texture( load("res/border.png") )
    patch_rect:set_margin( BORDER.TOP , 2  )
    patch_rect:set_margin( BORDER.BOTTOM , 2  )
    patch_rect:set_margin( BORDER.LEFT , 2  )
    patch_rect:set_margin( BORDER.RIGHT , 2  )

    child1 = SceneNode.new()
    child_rect = child1:create_patch_rect()
    this:add_child(child1)
    child_rect:set_texture( load("res/border.png") )
    child_rect:set_rect_pos(Vector2(0,0))
    child_rect:set_rect_size(Vector2(50,50))
    child_rect:set_draw_center(false)
    child_rect:set_starts_on_viewport(true)
    child_rect:set_use_scene_node_transform(false)


    child2 = SceneNode.new()
    label_rect = child2:create_label_rect() 
    child1:add_child(child2);
    label_rect:set_text([[Eu, que já andei pelos quatro cantos do mundo procurando Foi justamente num sonho que Ele me falou Às vezes você me]]);
    --label_rect:set_text([[O blefe do jogador Eu sou, eu fui, eu vou Eu sou o seu sacrifício A placa de contra-mão O sangue no olhar do vam]]);
    label_rect:set_font(load("res/fonts/Mono10_v0.95/Mono10 Regular.ttf"));
    label_rect:set_rect_pos(Vector2(10,10))
    label_rect:set_rect_size(Vector2(30,30))
    label_rect:set_anchored_border( BORDER.TOP , BORDER.TOP , true )
    label_rect:set_anchored_border( BORDER.LEFT , BORDER.LEFT , true )
    label_rect:set_anchored_border( BORDER.RIGHT , BORDER.RIGHT , true )
    label_rect:set_anchored_border( BORDER.BOTTOM , BORDER.BOTTOM , true )
    --label_rect:set_anchored_border( BORDER.LEFT , BORDER.RIGHT , true )

end

function _frame_start( dt_seconds )
    local id = this:get_saucer_id()
    local r = math.sin( id*123 );
    t = Engine.get_uptime()
    if not this:get_body() then
        local s = math.sin( Engine.get_uptime() * 200.0 + id*13 );
        this:set_rotation_degrees( this:get_rotation_degrees() + 1 )
    end

    child_patch_rect = this:get_children()[1]:get_patch_rect()
    child_patch_rect:set_rect_size( Vector2(
        300 + 200 * math.sin( t * 50 ),
        400 + 100 * math.cos( t * 230 )
    ) )

end

list_of_sounds = {
    load("res/sounds/boom.wav")
}



function _collision_start( other )
    local audio_emitter = this:get_audio_emitter()
    if not audio_emitter then
        audio_emitter = this:create_audio_emitter()
        i = math.random(1,table.getn(this.list_of_sounds))
        audio_emitter:set_audio_resource( this.list_of_sounds[i] )
    end
    -- audio_emitter:play()
    sprite = this:get_sprite()
    sprite:set_frame_index( sprite:get_frame_index()+1 )
    -- this:get_parent():get_out()
end
