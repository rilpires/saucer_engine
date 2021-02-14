#ifndef SAUCER_TYPE_TRAITS_H
#define SAUCER_TYPE_TRAITS_H

#include <vector>

class SaucerObject;

template< typename T>
struct is_vector {
    static const bool value = false;
};
template< typename T , typename T_alloc >
struct is_vector< const std::vector<T,T_alloc>& > {
    static const bool value = true;
    using value_type = T;
};
template< typename T , typename T_alloc >
struct is_vector< const std::vector<T,T_alloc> > {
    static const bool value = true;
    using value_type = T;
};
template< typename T , typename T_alloc >
struct is_vector< std::vector<T,T_alloc> > {
    static const bool value = true;
    using value_type = T;
};


template< typename T> 
struct function_member_unconstantizer;
template< typename ret_type , typename class_type , typename ... args_type> 
struct function_member_unconstantizer<ret_type(class_type::*)(args_type ...)>{
    using type = ret_type(class_type::*)(args_type ...);
};
template< typename ret_type , typename class_type , typename ... args_type> 
struct function_member_unconstantizer<ret_type(class_type::*)(args_type ...) const>{
    using type = ret_type(class_type::*)(args_type ...);
};
template< typename ret_type , typename ... args_type> 
struct function_member_unconstantizer<ret_type(args_type ...)>{
    using type = ret_type(args_type ...);
};

template< typename T> 
struct function_ret_type;
template< typename ret_type , typename class_type , typename ... args_type> 
struct function_ret_type<ret_type(class_type::*)(args_type ...)>{
    using type = ret_type;
};
template< typename ret_type , typename class_type , typename ... args_type> 
struct function_ret_type<ret_type(class_type::*)(args_type ...) const>{
    using type = ret_type;
};
template< typename ret_type , typename ... args_type> 
struct function_ret_type<ret_type(args_type ...)>{
    using type = ret_type;
};


template< typename T, class = typename std::enable_if< !std::is_pointer<T>::value >::type >
T& to_ref( T& obj ){ return obj; };
template< typename T, class = typename std::enable_if< !std::is_pointer<T>::value >::type >
T& to_ref( T* obj){ return *obj; };

template< typename T , class = void >
struct to_used_type;

#include "input.h"
template< typename T  >
struct to_used_type< T , typename std::enable_if< !std::is_base_of<SaucerObject,T>::value >::type >{
    using type = T;
};
template< typename T  >
struct to_used_type< T , typename std::enable_if< std::is_base_of<SaucerObject,T>::value >::type >{
    using type = T*;
};
template<>
struct to_used_type<Input::InputEvent>{
    using type = Input::InputEvent*;
};


#endif