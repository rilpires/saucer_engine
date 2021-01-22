#include "editor.h"
#include "core.h"

using namespace ImGui;

SaucerId SaucerEditor::node_id_selected = 0;

void SaucerEditor::setup(){
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO& io = GetIO(); (void)io;
    StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL( Engine::get_render_engine()->get_glfw_window(), true );
    ImGui_ImplOpenGL3_Init("#version 150");
}

void tree_recursive( SceneNode* node ){
    bool b = TreeNodeEx(    (void*)node->get_saucer_id(),
                            ImGuiTreeNodeFlags_OpenOnArrow + ImGuiTreeNodeFlags_SpanFullWidth , 
                            node->get_name().c_str() );
    if( IsItemClicked() ) SaucerEditor::node_id_selected = node->get_saucer_id();
    if( b ){
        for( auto child : node->get_children() ) tree_recursive(child);
        TreePop();
    }
}

#define PROPERTY_VEC2(obj,prop_name)\
    Vector2 prop_name = obj->get_##prop_name(); if( InputFloat2(#prop_name,(float*)&prop_name,"%.2f") ) obj->set_##prop_name(prop_name);
#define PROPERTY_FLOAT(obj,prop_name)\
    float prop_name = obj->get_##prop_name(); if( InputFloat(#prop_name,&prop_name,0,0,"%.2f") ) obj->set_##prop_name(prop_name);
#define PROPERTY_COLOR(obj,prop_name)\
    Color::ColorFloat prop_name = obj->get_##prop_name().to_float(); if( ColorEdit4(#prop_name,(float*)&prop_name,0) ) obj->set_##prop_name(prop_name.to_color());
#define PROPERTY_STRING(obj,prop_name)\
    std::string prop_name = obj->get_##prop_name(); if( InputText(#prop_name,&prop_name) ) obj->set_##prop_name(prop_name);
#define PROPERTY_INT(obj,prop_name)\
    int prop_name = obj->get_##prop_name(); if( InputInt(#prop_name,&prop_name) ) obj->set_##prop_name(prop_name);
#define PROPERTY_BOOL(obj,prop_name)\
    bool prop_name = obj->get_##prop_name(); if( Checkbox(#prop_name,&prop_name) ) obj->set_##prop_name(prop_name);


void inspector(){
    SceneNode* selected_node = (SceneNode*)SaucerObject::from_saucer_id(SaucerEditor::node_id_selected);
    if( selected_node ){
        Text("Name: %s" , selected_node->get_name().c_str() );
        PROPERTY_STRING(selected_node,name);
        PROPERTY_VEC2(selected_node,position);
        PROPERTY_VEC2(selected_node,scale);
        PROPERTY_FLOAT(selected_node,rotation_degrees);
        PROPERTY_COLOR(selected_node,modulate);
        PROPERTY_COLOR(selected_node,self_modulate);
        PROPERTY_INT(selected_node,z);
        PROPERTY_BOOL(selected_node,relative_z);
        PROPERTY_BOOL(selected_node,visible);
        PROPERTY_BOOL(selected_node,inherits_transform);
        
        NewLine();
        
        TextColored( ImVec4(0.8,1,0.8,1) , "Attached components:" );
        for( auto component : selected_node->get_attached_components() ){
            bool b = TreeNodeEx((void*)component->get_saucer_id() , 
                                ImGuiTreeNodeFlags_Framed + ImGuiTreeNodeFlags_SpanFullWidth ,
                                component->my_saucer_class_name() );
            if( b ){
                Text("Componente blablabl");
                if(Button("Delete component")) selected_node->destroy_component(component);
                TreePop();
            }
        }

        NewLine();
        
        if(Button("Create component")){
            OpenPopup("comps");
        }
        if( BeginPopup("comps") ){
            
            for( auto it : SceneNode::__get_component_constructors() ){
                if( MenuItem(it.first.c_str()) ){
                    (selected_node->*it.second)();
                }
            }

            EndPopup();
        }
    }
}

void SaucerEditor::update(){

    SceneNode* root_node = Engine::get_current_scene()->get_root_node();
    SceneNode* selected_node = (SceneNode*)SaucerObject::from_saucer_id(SaucerEditor::node_id_selected);
    if( !root_node ) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();

    ShowDemoWindow();
    
    Begin("Scene tree" , 0 , ImGuiWindowFlags_NoMove + ImGuiWindowFlags_NoResize );   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    SetWindowPos( ImVec2(0,0) );
    SetWindowSize( ImVec2( 640 ,0) );
    
    
    if(Button("New node")){
        if( selected_node ) selected_node->add_child( new SceneNode() );
        else if(root_node ) root_node->add_child( new SceneNode() );        
    }
    SameLine();
    
    if(Button("Delete node") && selected_node) selected_node->queue_free();
    
    Columns(2,"scene_tree_and_inspector",true);
    SetColumnWidth(0,250);
    SetNextTreeNodeOpen(true);
    tree_recursive(root_node);
    NextColumn();
    inspector();
    End();

    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}
