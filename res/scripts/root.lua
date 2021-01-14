table_state = false
    
function _entered_tree()
    
    require("res/scripts/chess.lua")
    
    this:set_name("root!")
    this.table_state = CHESS.create_table_state()

    local chess_table = SceneNode.new()
    chess_table:set_name("chess_table")
    this:add_child(chess_table)
    local chess_table_sprite = chess_table:create_sprite()
    chess_table_sprite:set_texture(load("res/chess.png"))
    chess_table_sprite:set_region_top_left(Vector2(320,160))
    chess_table_sprite:set_region_bottom_right(Vector2(576,416))
    chess_table:set_position(Vector2(320,160))
    
    local camera = SceneNode.new()
    this:add_child(camera)
    camera:create_camera()
    camera:set_position(Vector2(320,240))
    camera:get_camera():set_active(true)

    for k,v in this.table_state.map do
        local piece = SceneNode.new()
        piece:set_script(load("res/scripts/piece.lua"))
        chess_table:add_child(piece)
        piece:setup_from_piece(v)
    end


end

function _frame_start(delta)
    print("FPS: " , Engine.get_fps() )
end