
function setup_from_piece( this , p )
    this.p = p;
    this.rect = this:get_patch_rect()
    this.child = this:get_node("piece child! (sprite)")
    
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
    this.child:get_sprite():set_frame_index(used_index)
    
end

function _entered_mouse()
    this.mouse_in = true
    this._frame_start = function(delta)
        local s = math.sin(2000*Engine.get_uptime())
        this.child:set_rotation_degrees( 10*s )
    end
end
function _exiting_mouse()
    this.mouse_in = false
    this._frame_start = nil
    this.child:set_rotation_degrees(0)
end

function chess_coords_from_pos()
    local p = this:get_position()
    if( p.x < 16 )          then p.x = 16           end
    if( p.x > 7*32 + 16 )   then p.x = 7*32 + 16    end
    if( p.y < 16 )          then p.y = 16           end;
    if( p.y > 7*32 + 16 )   then p.y = 7*32 + 16    end;
    local target_chess_x = math.floor(p.x/32)
    local target_chess_y = math.floor(p.y/32)
    return target_chess_x , target_chess_y
end

function _input( input )
    if( this.mouse_in and input:get_type() == InputEventType.MOUSE_BUTTON and input:is_pressed() and input:get_button()==0 and not input:is_echo() ) then
        this.being_dragged = true
        input:solve()
    elseif this.being_dragged and input:get_type() == InputEventType.MOUSE_MOTION then
        this:set_global_position( Input:get_world_mouse_position() )
        local target_chess_x , target_chess_y = this.chess_coords_from_pos()
        this:set_position(Vector2(target_chess_x,target_chess_y)*32 + Vector2(16,16))
    elseif this.being_dragged and input:get_type() == InputEventType.MOUSE_BUTTON and not input:is_pressed() and input:get_button()==0 then
        this.being_dragged = false
        local target_chess_x , target_chess_y = this.chess_coords_from_pos()
        local target_chess_pos = target_chess_x+8*target_chess_y
        local current_chess_pos = this.p.x + 8*this.p.y
        if( this.table_state:is_move_allowed(current_chess_pos,target_chess_pos) ) then
            this.table_state:move( current_chess_pos , target_chess_pos )
            this:set_position(Vector2(this.p.x,this.p.y)*32 + Vector2(16,16))
        end
        this:set_position(Vector2(this.p.x,this.p.y)*32 + Vector2(16,16))
    end
end

