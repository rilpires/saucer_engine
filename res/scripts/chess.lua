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
        ret.white_time = 600.00
        ret.black_time = 600.00
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
        ret.pass_time = CHESS.pass_time

        return ret
    end,

    is_move_allowed = function( table_state , from , to )
        local source_piece = table_state.map[from]
        local target_piece = table_state.map[to]

        if( from == to ) then return false end;

        if not source_piece then
            LOG:log("There is no piece being moved!")
            return false
        end
        
        if ( source_piece.color ~= table_state.current_player ) then
            LOG:log("This isn't this player turn to move.")
            return false
        end

        if ( target_piece and source_piece.color == target_piece.color ) then
            LOG:log("You can't domain your own piece!")
            return false
        end
        
        for k,v in CHESS.get_allowed_destinations(table_state,source_piece) do
            if( v == to ) then 
                return true 
            end
        end
        LOG:log("This movement is not allowed for this unit...")
        return false
    end,

    move = function( table_state , from , to )
        local p1 = table_state.map[from]
        local p2 = table_state.map[to]
        if( p2 ) then
            if( table_state._dead_piece ) then
                table_state._dead_piece( p2 )
            end
        end
        if( not p1 ) then return end
        table_state.map[to] = p1;
        table_state.map[from] = nil;
        p1.x = math.mod(to,8)
        p1.y = math.floor(to/8)
        if( table_state.current_player == CHESS.COLOR.WHITE ) then
            table_state.current_player = CHESS.COLOR.BLACK
        else 
            table_state.current_player = CHESS.COLOR.WHITE
        end
    end,

    get_allowed_destinations = function( table_state , piece )
        local ret = {}
        local p1 = piece
        local x = p1.x
        local y = p1.y
        
        if( p1.type == CHESS.PIECE_TYPE.TOWER ) then
            for dx=x+1,7,1 do
                local p2 = table_state.map[dx + 8*y ]
                if      (not p2)             then table.insert(ret,dx+8*y)
                elseif  p1.color ~= p2.color then table.insert(ret,dx+8*y) break
                elseif  p1.color == p2.color then break end
            end
            for dx=x-1,0,-1 do
                local p2 = table_state.map[dx + 8*y ]
                if      (not p2)              then table.insert(ret,dx+8*y)
                elseif  p1.color ~= p2.color  then table.insert(ret,dx+8*y) break
                elseif  p1.color == p2.color  then break end
            end
            for dy=y+1,7,1 do
                local p2 = table_state.map[x + 8*dy ]
                if (not p2)                  then  table.insert(ret,x+8*dy)    
                elseif p1.color ~= p2.color  then  table.insert(ret,x+8*dy) break   
                elseif p1.color == p2.color  then  break end
            end
            for dy=y-1,0,-1 do
                local p2 = table_state.map[x + 8*dy ]
                if (not p2)                 then table.insert(ret,x+8*dy)
                elseif p1.color ~= p2.color then table.insert(ret,x+8*dy) break
                elseif p1.color == p2.color then break end
            end
        elseif ( p1.type == CHESS.PIECE_TYPE.BISHOP ) then
            for delta=1,7,1 do
                local dx = x + delta
                local dy = y + delta
                if( dx>=0 and dx<=7 and dy>=0 and dy<=7 ) then
                    local p2 = table_state.map[dx+8*dy]
                    if (not p2)                 then table.insert(ret,dx+8*dy)
                    elseif p1.color ~= p2.color then table.insert(ret,dx+8*dy) break 
                    elseif p1.color == p2.color then break end   
                end
            end
            for delta=1,7,1 do
                local dx = x + delta
                local dy = y - delta
                if( dx>=0 and dx<=7 and dy>=0 and dy<=7 ) then
                    local p2 = table_state.map[dx+8*dy]
                    if (not p2)                 then table.insert(ret,dx+8*dy)
                    elseif p1.color ~= p2.color then table.insert(ret,dx+8*dy) break 
                    elseif p1.color == p2.color then break end   
                end
            end
            for delta=1,7,1 do
                local dx = x - delta
                local dy = y + delta
                if( dx>=0 and dx<=7 and dy>=0 and dy<=7 ) then
                    local p2 = table_state.map[dx+8*dy]
                    if (not p2)                 then table.insert(ret,dx+8*dy)
                    elseif p1.color ~= p2.color then table.insert(ret,dx+8*dy) break 
                    elseif p1.color == p2.color then break end   
                end
            end
            for delta=1,7,1 do
                local dx = x - delta
                local dy = y - delta
                if( dx>=0 and dx<=7 and dy>=0 and dy<=7 ) then
                    local p2 = table_state.map[dx+8*dy]
                    if (not p2)                 then table.insert(ret,dx+8*dy)
                    elseif p1.color ~= p2.color then table.insert(ret,dx+8*dy) break 
                    elseif p1.color == p2.color then break end   
                end
            end

        elseif ( p1.type == CHESS.PIECE_TYPE.QUEEN ) then
            local fake_p1 = {x=p1.x,y=p1.y,color=p1.color}
            fake_p1.type = CHESS.PIECE_TYPE.TOWER
            for k,v in CHESS.get_allowed_destinations( table_state , fake_p1 ) do 
                table.insert(ret,v)
            end
            fake_p1.type = CHESS.PIECE_TYPE.BISHOP
            for k,v in CHESS.get_allowed_destinations( table_state , fake_p1 ) do 
                table.insert(ret,v)
            end
        elseif ( p1.type == CHESS.PIECE_TYPE.KING ) then
            for dx=x-1,x+1 do
                for dy=y-1,y+1 do
                    if( not (dx==0 and dy==0) and dx>=0 and dx<=7 and dy>=0 and dy<=7 ) then
                        local p2 = table_state.map[dx+8*dy]
                        if( (not p2) or (p1.color ~= p2.color) ) then
                            table.insert(ret,dx+8*dy)
                        end
                    end
                end
            end
        elseif ( p1.type == CHESS.PIECE_TYPE.KNIGHT ) then
            for k,delta_x in {-2,-1,1,2} do
                for k,delta_y in {-2,-1,1,2} do
                    local dx = x + delta_x;
                    local dy = y + delta_y;
                    if( dx>=0 and dx<=7 and dy>=0 and dy<=7 and math.abs(delta_x)~=math.abs(delta_y) ) then 
                        local p2 = table_state.map[dest]
                        if( (not p2) or (p1.color ~= p2.color) ) then
                            table.insert(ret,dx+8*dy)
                        end
                    end
                end
            end
        elseif ( p1.type == CHESS.PIECE_TYPE.PEON ) then 
            local first_movement = false
            first_movement = (p1.color==CHESS.COLOR.BLACK and p1.y==1 ) or (p1.color==CHESS.COLOR.WHITE and p1.y==6 )
            if( p1.color==CHESS.COLOR.BLACK ) then
                if( y < 7 ) then
                    local p2 = table_state.map[x+(y+1)*8]
                    if not p2 then table.insert(ret,x+(y+1)*8) end
                    if( x > 0 ) then
                        local p2 = table_state.map[x-1+(y+1)*8]
                        if( p2 and p2.color ~= p1.color ) then table.insert(ret,x-1+(y+1)*8) end
                    end
                    if( x < 7 ) then
                        local p2 = table_state.map[x+1+(y+1)*8]
                        if( p2 and p2.color ~= p1.color ) then table.insert(ret,x+1+(y+1)*8) end
                    end
                end
                if( first_movement ) then
                    local p2 = table_state.map[x+(y+2)*8]
                    if not p2 then table.insert(ret,x+(y+2)*8) end
                end
            end
            if( p1.color==CHESS.COLOR.WHITE ) then
                if( y > 0 ) then
                    local p2 = table_state.map[x+(y-1)*8]
                    if not p2 then table.insert(ret,x+(y-1)*8) end
                    if( x > 0 ) then
                        local p2 = table_state.map[x-1+(y-1)*8]
                        if( p2 and p2.color ~= p1.color ) then table.insert(ret,x-1+(y-1)*8) end
                    end
                    if( x < 7 ) then
                        local p2 = table_state.map[x+1+(y-1)*8]
                        if( p2 and p2.color ~= p1.color ) then table.insert(ret,x+1+(y-1)*8) end
                    end
                end
                if( first_movement ) then
                    local p2 = table_state.map[x+(y-2)*8]
                    if not p2 then table.insert(ret,x+(y-2)*8) end
                end
            end
            
        end
        
        
        return ret
    end ,

    get_winner = function( table_state ) 
        return nil
    end,

    pass_time = function( table_state , delta_time ) 
        if (table_state.current_player == CHESS.COLOR.WHITE) then
            table_state.white_time = math.max( table_state.white_time-delta_time , 0 )
        elseif (table_state.current_player == CHESS.COLOR.BLACK) then
            table_state.black_time = math.max( table_state.black_time-delta_time , 0 )
        end
    end

}
