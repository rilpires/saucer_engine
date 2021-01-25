table_state = false

function _entered_tree()
    require("res/scripts/chess.lua")
    this.table_state = CHESS.create_table_state()
    this.player_color = CHESS.COLOR.WHITE
    this.table_state._dead_piece = this.dead_piece
    LOG = this:get_node("chat")

    local piece_index = 1;
    for k,piece in this.table_state.map do
        local piece_node = this:get_node("chess_table"):get_children()[piece_index];
        piece_node:setup_from_piece( piece )
        piece.node = piece_node
        piece_node.table_state = this.table_state
        piece_index = piece_index+1
    end
end

next_frame_to_inform_fps = 0
function _frame_start(delta)
    if( this.table_state ) then
        this.table_state:pass_time( 15*delta )

        if (this.player_color == CHESS.COLOR.WHITE) then
            this:get_node("player_timer"):set_seconds(   this.table_state.white_time )
            this:get_node("opponent_timer"):set_seconds( this.table_state.black_time )
        elseif (this.player_color == CHESS.COLOR.BLACK) then 
            this:get_node("player_timer"):set_seconds(   this.table_state.black_time )
            this:get_node("opponent_timer"):set_seconds( this.table_state.white_time )
        end
    end
end

function dead_piece( p )
    print(p , " morreu!")
    p.node:queue_free()
end

