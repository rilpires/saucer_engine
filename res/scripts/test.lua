
function frame_start( dt_seconds )
    pos = get_position(_NODE_ID);
    print("my _NODE_ID is " , _NODE_ID )
    print("my position is " , get_position(_NODE_ID) )
    print(string.format( "pos.x = %f , pos.y = %f" , pos.x , pos.y ))
    pos.x = pos.x + 1
    print(string.format( "now it is: pos.x = %f , pos.y = %f" , pos.x , pos.y ))
    -- if( _G['_SAUCER'] ) then
    --     for k,v in pairs(_G['_SAUCER']) do
    --         print("k = " , k )
    --         print("v = " , v ) 
    --     end    
    --     if( _G['_SAUCER']['_NODES'] ) then
    --         print("nodes:--------")
    --         for k,v in pairs(_G['_SAUCER']['_NODES']) do
    --             print("k = " , k )
    --             print("v = " , v )
    --             for k2,v2 in pairs(_G['_SAUCER']['_NODES'][k]) do
    --                 print("k2 = " , k2  , type(k2) )
    --                 print("v2 = " , v2  , type(v2) )
    --             end
    --         end
    --     end
    -- end
end

