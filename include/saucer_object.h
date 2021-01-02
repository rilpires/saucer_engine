#ifndef SAUCER_OBJECT_H
#define SAUCER_OBJECT_H

#include <unordered_map>


class SaucerObject {


    #define REGISTER_SAUCER_OBJECT(C)                   \
    public:                                             \
        static constexpr const char* class_name = #C;   \
    ;
 
    REGISTER_SAUCER_OBJECT(SaucerObject);

    public:
    using SaucerId = uint32_t ; 

    private:

    static SaucerId open_id;
    static std::unordered_map<SaucerId,SaucerObject*> saucer_objs;
    
    SaucerId    id;

    public:
    
    SaucerObject();
    SaucerId                get_saucer_id() const ;
    static SaucerObject*    from_saucer_id( const SaucerId& ); 


    protected:
    virtual ~SaucerObject();

};

typedef SaucerObject::SaucerId SaucerId;

#endif