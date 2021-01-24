#ifdef SAUCER_EDITOR
#ifndef SAUCER_EDITOR_H
#define SAUCER_EDITOR_H

#include "saucer_object.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#define PROPERTY_VEC2(obj,prop_name)    Vector2 prop_name = obj->get_##prop_name(); if( ImGui::DragFloat2(#prop_name,(float*)&prop_name,1.0f,0.0f,0.0f,"%.2f") ) obj->set_##prop_name(prop_name);
#define PROPERTY_FLOAT(obj,prop_name)   float prop_name = obj->get_##prop_name(); if( ImGui::DragFloat(#prop_name,&prop_name,1.0f,0.0f,0.0f,"%.2f") ) obj->set_##prop_name(prop_name);
#define PROPERTY_FLOAT_RANGE(obj,prop_name,min,max)   float prop_name = obj->get_##prop_name(); if( ImGui::SliderFloat(#prop_name,&prop_name,min,max,"%.2f") ) obj->set_##prop_name(prop_name);
#define PROPERTY_COLOR(obj,prop_name)   Color::ColorFloat prop_name = obj->get_##prop_name().to_float(); if( ImGui::ColorEdit4(#prop_name,(float*)&prop_name,0) ) obj->set_##prop_name(prop_name.to_color());
#define PROPERTY_STRING(obj,prop_name)  std::string prop_name = obj->get_##prop_name(); if( ImGui::InputText(#prop_name,&prop_name,ImGuiInputTextFlags_EnterReturnsTrue) ) obj->set_##prop_name(prop_name);
#define PROPERTY_INT(obj,prop_name)     int prop_name = obj->get_##prop_name(); if( ImGui::InputInt(#prop_name,&prop_name) ) obj->set_##prop_name(prop_name);
#define PROPERTY_BOOL(obj,prop_name)    bool prop_name = obj->get_##prop_name(); if( ImGui::Checkbox(#prop_name,&prop_name) ) obj->set_##prop_name(prop_name);
#define PROPERTY_RECT(obj,prop_name)   Rect prop_name = obj->get_##prop_name(); if( ImGui::DragFloat4(#prop_name,(float*)&prop_name,1.0f,0.0f,0.0f,"%.2f") ) obj->set_##prop_name(prop_name);
#define PROPERTY_RESOURCE(obj,PROPERTY_NAME,RESOURCE_TYPE)                                                          \
    std::string PROPERTY_NAME##_path;                                                                               \
    if( obj->get_##PROPERTY_NAME() ) PROPERTY_NAME##_path = obj->get_##PROPERTY_NAME()->get_path();                 \
    if( ImGui::InputText( #PROPERTY_NAME" path" , &PROPERTY_NAME##_path , ImGuiInputTextFlags_EnterReturnsTrue ) )  \
        obj->set_##PROPERTY_NAME( ResourceManager::get_resource<RESOURCE_TYPE>(PROPERTY_NAME##_path) );             

class SceneNode;

namespace SaucerEditor {

    extern SaucerId node_id_selected;
    
    void push_scene_tree_window();
    void push_node_tree( SceneNode* node );
    void push_render_window();
    void push_inspector();
    void setup();
    void update();
    
};


#endif
#else 
// If SAUCER_EDITOR is not defined, still gotta fill those macros

#define PROPERTY_VEC2(obj,prop_name) ;
#define PROPERTY_FLOAT(obj,prop_name) ;
#define PROPERTY_COLOR(obj,prop_name) ;
#define PROPERTY_STRING(obj,prop_name) ;
#define PROPERTY_INT(obj,prop_name) ;
#define PROPERTY_BOOL(obj,prop_name) ;


#endif
