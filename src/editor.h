#ifndef SAUCER_EDITOR_H
#define SAUCER_EDITOR_H

#ifdef SAUCER_EDITOR


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "saucer_object.h"

class SceneNode;

namespace SaucerEditor {

    struct EditorStream : public std::streambuf , std::ostream  {
        private:
            std::string str;
        public:
            EditorStream() : std::ostream(this) {};

            int overflow (int);
            const char* get_buffer() const ;
            size_t  get_size() const ;
            void    clear();
    };

    extern EditorStream     stream;
    extern SaucerId         node_id_selected;
    extern SceneNode*       tree_root_before_play;
    extern bool             currently_playing;
    extern std::unordered_map<SaucerId,std::string> reference_path;
    extern std::unordered_map<std::string, std::vector<std::string> > resources_by_type;

    // Called once
    void setup();

    // Helpers
    void push_scene_tree_window();
    void push_node_tree( SceneNode* node );
    void push_render_window();
    void push_inspector();
    void push_config();
    void push_lua_profiler();
    void push_lua_editor();
    void push_engine_profiler();
    void push_console();
    void push_resource_explorer();
    bool push_resource_hint( std::string resource_type , std::string& path );

    std::string get_reference_path( const SceneNode* );
    void flag_as_referenced( const SceneNode* , std::string );

    void DefinePopupSaveNode( SceneNode* , std::string& );

    // called by Engine every frame
    void update();
    
};


#define PROPERTY_VEC2(obj,prop_name)    Vector2 prop_name = obj->get_##prop_name(); if( ImGui::DragFloat2(#prop_name,(float*)&prop_name,1.0f,0.0f,0.0f,"%.2f") ) obj->set_##prop_name(prop_name);
#define PROPERTY_FLOAT(obj,prop_name)   float prop_name = obj->get_##prop_name(); if( ImGui::DragFloat(#prop_name,&prop_name,1.0f,0.0f,0.0f,"%.2f") ) obj->set_##prop_name(prop_name);
#define PROPERTY_FLOAT_RANGE(obj,prop_name,min,max)   float prop_name = obj->get_##prop_name(); if( ImGui::SliderFloat(#prop_name,&prop_name,min,max,"%.2f") ) obj->set_##prop_name(prop_name);
#define PROPERTY_COLOR(obj,prop_name)   Color::ColorFloat prop_name = obj->get_##prop_name().to_float(); if( ImGui::ColorEdit4(#prop_name,(float*)&prop_name,0) ) obj->set_##prop_name(prop_name.to_color());
#define PROPERTY_STRING(obj,prop_name)  std::string prop_name = obj->get_##prop_name(); if( ImGui::InputText(#prop_name,&prop_name,ImGuiInputTextFlags_EnterReturnsTrue) ) obj->set_##prop_name(prop_name);
#define PROPERTY_INT(obj,prop_name)     int prop_name = obj->get_##prop_name(); if( ImGui::InputInt(#prop_name,&prop_name) ) obj->set_##prop_name(prop_name);
#define PROPERTY_BOOL(obj,prop_name)    bool prop_name = obj->get_##prop_name(); if( ImGui::Checkbox(#prop_name,&prop_name) ) obj->set_##prop_name(prop_name);
#define PROPERTY_RECT(obj,prop_name)   Rect prop_name = obj->get_##prop_name(); if( ImGui::DragFloat4(#prop_name,(float*)&prop_name,1.0f,0.0f,0.0f,"%.2f") ) obj->set_##prop_name(prop_name);
#define PROPERTY_RESOURCE(obj,PROPERTY_NAME,RESOURCE_TYPE)                                              \
                std::string path;                                                                       \
                if( obj->get_##PROPERTY_NAME() ) path = obj->get_##PROPERTY_NAME()->get_path();         \
                else path = "None";                                                                     \
                if( ImGui::Button(path.c_str()) ){                                                      \
                    ImGui::OpenPopup(#PROPERTY_NAME);                                                   \
                }                                                                                       \
                ImGui::SameLine();                                                                      \
                ImGui::Text(#PROPERTY_NAME);                                                            \
                if(ImGui::BeginPopup(#PROPERTY_NAME)){                                                  \
                    if( SaucerEditor::push_resource_hint(RESOURCE_TYPE::class_name , path) ){           \
                        obj->set_##PROPERTY_NAME( ResourceManager::get_resource<RESOURCE_TYPE>(path) ); \
                        ImGui::CloseCurrentPopup();                                                     \
                    }                                                                                   \
                    ImGui::EndPopup();                                                                  \
                } 
#define PROPERTY_ENUM(obj,prop_name,map)                                                                        \
                if(ImGui::BeginPopup("select_"#prop_name)){                                                     \
                    for( auto p : map ) if( ImGui::Selectable(p.second.c_str()) )                               \
                            obj->set_##prop_name(p.first);                                                      \
                    ImGui::EndPopup();                                                                          \
                }                                                                                               \
                ImGui::Text(#prop_name":"); ImGui::SameLine();                                                  \
                if( ImGui::Button(map[obj->get_##prop_name()].c_str()))  ImGui::OpenPopup("select_"#prop_name); 

#else 
// If SAUCER_EDITOR is not defined, still gotta fill those macros

#include <string>

namespace SaucerEditor {
    void setup();
    extern std::string current_scene_path;
}

#define PROPERTY_VEC2(obj,prop_name) ;
#define PROPERTY_FLOAT(obj,prop_name) ;
#define PROPERTY_FLOAT_RANGE(obj,prop_name,a,b) ;
#define PROPERTY_COLOR(obj,prop_name) ;
#define PROPERTY_STRING(obj,prop_name) ;
#define PROPERTY_INT(obj,prop_name) ;
#define PROPERTY_BOOL(obj,prop_name) ;
#define PROPERTY_RECT(obj,prop_name) ;
#define PROPERTY_RESOURCE(obj,PROPERTY_NAME,RESOURCE_TYPE) ;          
#define PROPERTY_ENUM(obj,prop_name,map) ;

#endif
#endif
