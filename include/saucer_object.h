#ifndef SAUCER_OBJECT_H
#define SAUCER_OBJECT_H

#include <unordered_map>
#include <vector>


class SaucerObject {


    #define REGISTER_SAUCER_OBJECT(Class,ParentClass)\
    public:                                                                         \
        static constexpr const char* parent_class_name = ParentClass::class_name;   \
        static constexpr const char* class_name = #Class;                           \
        using parent_type = ParentClass;                                            \
    ;

    public:
        using SaucerId = uint32_t ; 
        static constexpr const char* parent_class_name  = "";  
        static constexpr const char* class_name         = "SaucerObject";
        using parent_type = SaucerObject;
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