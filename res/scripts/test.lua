pessoa = {}
pessoa.nome = "Romeu"
pessoa.idade = 26

print("_G has _SAUCER? " , _G['_SAUCER']~=nil )
if( _G['_SAUCER'] ) then
    for k,v in pairs(_G['_SAUCER']) do
        print("k = " , k )
        print("v = " , v ) 
    end    
    if( _G['_SAUCER']['_NODES'] ) then
        print("nodes:--------")
        for k,v in pairs(_G['_SAUCER']['_NODES']) do
            print("k = " , k )
            print("v = " , v )
        end
    end
end



