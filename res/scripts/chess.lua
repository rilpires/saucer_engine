CHESS = {

    COLOR = {
        WHITE=1,
        BLACK=2
    },

    PIECE_TYPE = {
        PEON = 1 ,
        TOWER = 2 ,
        BISHOP = 3 ,
        KNIGHT = 4 ,
        QUEEN = 5 ,
        KING = 6
    },

    create_table_state = function ()
        local ret = {}
        ret.current_player = CHESS.COLOR.WHITE
        ret.width = 8
        ret.height = 8
        ret.map = {}
        for x=0,7 do
            ret.map[ x + 1*ret.width ] = { color = CHESS.COLOR.BLACK , type = CHESS.PIECE_TYPE.PEON , x=x , y=1 }
            ret.map[ x + 6*ret.width ] = { color = CHESS.COLOR.WHITE , type = CHESS.PIECE_TYPE.PEON , x=x , y=6 } 
        end
        ret.map[0] = { color = CHESS.COLOR.BLACK , type = CHESS.PIECE_TYPE.TOWER  , x=0 , y=0 }
        ret.map[7] = { color = CHESS.COLOR.BLACK , type = CHESS.PIECE_TYPE.TOWER  , x=7 , y=0 }
        ret.map[56] = { color = CHESS.COLOR.WHITE , type = CHESS.PIECE_TYPE.TOWER , x=0 , y=7 }
        ret.map[63] = { color = CHESS.COLOR.WHITE , type = CHESS.PIECE_TYPE.TOWER , x=7 , y=7 }
        
        ret.map[1] = { color = CHESS.COLOR.BLACK , type = CHESS.PIECE_TYPE.KNIGHT  , x=1 , y=0 }
        ret.map[6] = { color = CHESS.COLOR.BLACK , type = CHESS.PIECE_TYPE.KNIGHT  , x=6 , y=0 }
        ret.map[57] = { color = CHESS.COLOR.WHITE , type = CHESS.PIECE_TYPE.KNIGHT , x=1 , y=7 }
        ret.map[62] = { color = CHESS.COLOR.WHITE , type = CHESS.PIECE_TYPE.KNIGHT , x=6 , y=7 }
        
        ret.map[2] = { color = CHESS.COLOR.BLACK , type = CHESS.PIECE_TYPE.BISHOP  , x=2 , y=0 }
        ret.map[5] = { color = CHESS.COLOR.BLACK , type = CHESS.PIECE_TYPE.BISHOP  , x=5 , y=0 }
        ret.map[58] = { color = CHESS.COLOR.WHITE , type = CHESS.PIECE_TYPE.BISHOP , x=2 , y=7 }
        ret.map[61] = { color = CHESS.COLOR.WHITE , type = CHESS.PIECE_TYPE.BISHOP , x=5 , y=7 }
        
        
        ret.map[3] = { color = CHESS.COLOR.BLACK , type = CHESS.PIECE_TYPE.QUEEN   , x=3 , y=0 }
        ret.map[4] = { color = CHESS.COLOR.BLACK , type = CHESS.PIECE_TYPE.KING    , x=4 , y=0 }
        ret.map[59] = { color = CHESS.COLOR.WHITE , type = CHESS.PIECE_TYPE.QUEEN  , x=3 , y=7 }
        ret.map[60] = { color = CHESS.COLOR.WHITE , type = CHESS.PIECE_TYPE.KING   , x=4 , y=7 }
        
        ret.get_allowed_destinations = CHESS.get_allowed_destinations
        ret.is_move_allowed = CHESS.is_move_allowed
        ret.get_winner = CHESS.get_winner
        ret.move = CHESS.move

        return ret
    end,

    is_move_allowed = function( table_state , from , to )
        local source_piece = table_state.map[from]
        local target_piece = table_state.map[to]
        if( (not source_piece) or 
        ( source_piece.color ~= table_state.current_player ) or 
        ( target_piece and source_piece.color == target_piece.color ) ) then
            return false;
        end
        
        for k,v in table_state:get_allowed_destinations(from) do
            if( v == to ) then 
                return true 
            end
        end

        return false
    end,

    move = function( table_state , from , to )
        print("faz nada")
    end,

    get_allowed_destinations = function( table_state , piece_location )
        local ret = {}
        local p1 = table_state.map[piece_location]
        
        if( p1.type == CHESS.PIECE_TYPE.TOWER ) then
            local x = math.mod(piece_location,8)
            local y = math.floor(piece_location/8)
            for dx=x+1,7,1 do
                local p2 = table_state.map[dx + 8*y ]
                if (not p2) then
                    table.insert(ret,dx+8*y)
                elseif p1.color ~= p2.color then
                    table.insert(ret,dx+8*y)
                    break
                elseif p1.color == p2.color then
                    break
                end
            end
            for dx=x-1,0,-1 do
                local p2 = table_state.map[dx + 8*y ]
                if (not p2) then
                    table.insert(ret,dx+8*y)
                elseif p1.color ~= p2.color then
                    table.insert(ret,dx+8*y)
                    break
                elseif p1.color == p2.color then
                    break
                end
            end
            for dy=y+1,7,1 do
                local p2 = table_state.map[x + 8*dy ]
                if (not p2) then
                    table.insert(ret,x+8*dy)
                elseif p1.color ~= p2.color then
                    table.insert(ret,x+8*dy)
                    break
                elseif p1.color == p2.color then
                    break
                end
            end
            for dy=y-1,0,-1 do
                local p2 = table_state.map[x + 8*dy ]
                if (not p2) then
                    table.insert(ret,x+8*dy)
                elseif p1.color ~= p2.color then
                    table.insert(ret,x+8*dy)
                    break
                elseif p1.color == p2.color then
                    break
                end
            end
        elseif ( p1.type == CHESS.PIECE_TYPE.BISHOP ) then
        end

        return ret
    end ,

    get_winner = function( table_state ) 
        return nil
    end,


}
