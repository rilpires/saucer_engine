
function setup_from_piece( this , p )
    this:set_name("piece!")
    this:set_position( Vector2( p.x*32 , p.y*32 ) )

    local sprite = this:create_sprite()
    sprite:set_texture( load("res/chess.png") )
    sprite:set_h_frames(20)
    sprite:set_v_frames(15)
    local base_index = 7 + 5*20;
    local used_index = base_index
    if( p.color == CHESS.COLOR.BLACK ) then
        used_index = used_index+1;
    end
    local t = { [CHESS.PIECE_TYPE.KNIGHT] = 0*20,
                [CHESS.PIECE_TYPE.TOWER] = 1*20,
                [CHESS.PIECE_TYPE.BISHOP] = 2*20,
                [CHESS.PIECE_TYPE.QUEEN] = 3*20,
                [CHESS.PIECE_TYPE.KING] = 4*20,
                [CHESS.PIECE_TYPE.PEON] = 5*20 }
    used_index = used_index + t[p.type]
    sprite:set_frame_index(used_index)
end
