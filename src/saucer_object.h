#ifndef SAUCER_OBJECT_H
#define SAUCER_OBJECT_H

#include <unordered_map>
#include <vector>
#include "debug.h"
#include "yaml-cpp/yaml.h"

#define UNUSED(expr) (void)(expr)

extern uint32_t __open_saucer_class_id;
std::vector<void*>& __class_bind_methods();

template<typename T>
uint32_t get_next_saucer_class_id();


template<typename T>
struct SaucerClassId {
    static const uint32_t value;
};
template<typename T>
const uint32_t SaucerClassId<T>::value = get_next_saucer_class_id<T>(); 

typedef YAML::Node YamlNode; 

class SaucerObject {

    #define REGISTER_SAUCER_OBJECT(Class,ParentClass)\
    public:                                                                                             \
        static constexpr const char* parent_class_name = ParentClass::class_name;                       \
        static constexpr const char* class_name = #Class;                                               \
        using parent_type = ParentClass;                                                                \
        virtual uint32_t my_saucer_class_id() const { return SaucerClassId<Class>::value; }             \
        virtual const char* my_saucer_class_name() const { return #Class; }                             \
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
        static void             bind_methods();

        virtual YamlNode        to_yaml_node() const ;
        virtual void            from_yaml_node( YamlNode );
        void                    from_yaml_node( std::string );
        void                    save_as_file( std::string ) const;

        virtual uint32_t my_saucer_class_id() const { return SaucerClassId<SaucerObject>::value; }
        virtual const char* my_saucer_class_name() const { return class_name; }

    protected:
        virtual ~SaucerObject();

};

typedef SaucerObject::SaucerId SaucerId;


template<typename T>
uint32_t get_next_saucer_class_id(){
    __class_bind_methods().push_back( (void*)(T::bind_methods) );
    saucer_print("Generating class id = " , __open_saucer_class_id , " for class " , T::class_name );
    return __open_saucer_class_id++;
}


#endif