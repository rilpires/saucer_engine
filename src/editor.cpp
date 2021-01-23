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
            bool b = TreeNodeEx((void*)(long long)component->get_saucer_id() , 
                                ImGuiTreeNodeFlags_Framed + ImGuiTreeNodeFlags_SpanFullWidth ,
                                "%s" , component->my_saucer_class_name() );
            if( b ){
                component->push_editor_items();
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

void SaucerEditor::push_node_tree( SceneNode* node ){

    bool b = TreeNodeEx(    (void*)(long long)node->get_saucer_id(),
                            ImGuiTreeNodeFlags_OpenOnArrow + ImGuiTreeNodeFlags_SpanFullWidth + ImGuiTreeNodeFlags_Selected*(node->get_saucer_id()==SaucerEditor::node_id_selected) , 
                            "%s" , node->get_name().c_str() );
    if( IsItemClicked() ) SaucerEditor::node_id_selected = node->get_saucer_id();
    if( b ){
        for( auto child : node->get_children() ) push_node_tree(child);
        TreePop();
    }
}
void SaucerEditor::push_render_window(){

    SetNextWindowBgAlpha(0);
    Vector2 mouse_pos = Input::get_screen_mouse_position();
    Rect current_viewport_rect = Engine::get_render_engine()->get_viewport_rect();
    bool window_inside_viewport = current_viewport_rect.is_point_inside(mouse_pos);
    bool render_is_open = Begin( "Render" , 0 ,   ImGuiWindowFlags_NoScrollbar 
                                                + ImGuiWindowFlags_NoMouseInputs * window_inside_viewport
                                                + ImGuiWindowFlags_NoScrollWithMouse 
                                                + ImGuiWindowFlags_MenuBar );
    Vector2 available_sizes[] = {
        Vector2(320,240), 
        Vector2(640,480), 
        Vector2(800,600) , 
        Vector2(1024,768) , 
        Vector2(1280,720) , 
        Vector2(1366,768) 
    };
    BeginMenuBar();
    if( BeginMenu("Set size") ){
        for( auto v : available_sizes )
            if( MenuItem( ((std::string)v).c_str() ) ){
                Vector2 window_size = Vector2( GetWindowSize().x , GetWindowSize().y );
                Vector2 content_size = Vector2( GetContentRegionAvail().x , GetContentRegionAvail().y );
                Vector2 corrected_window_size = v + window_size - content_size;
                SetWindowSize( "Render" , ImVec2(corrected_window_size) );
            }
        EndMenu();
    }
    EndMenuBar();

    if(render_is_open) {
        Vector2 window_pos = Vector2( GetWindowPos().x , GetWindowPos().y);
        Vector2 content_pos = Vector2( GetWindowContentRegionMin().x , GetWindowContentRegionMin().y );
        Vector2 content_size = Vector2( GetContentRegionAvail().x , GetContentRegionAvail().y );
        Rect viewport_rect = Rect( window_pos+content_pos , window_pos+content_pos+content_size );
        Engine::get_render_engine()->set_viewport_rect(viewport_rect);
    } else
        Engine::get_render_engine()->set_viewport_rect(Rect());
    End();
}

void SaucerEditor::update(){

    SceneNode* root_node = Engine::get_current_scene()->get_root_node();
    SceneNode* selected_node = (SceneNode*)SaucerObject::from_saucer_id(SaucerEditor::node_id_selected);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();

    ShowDemoWindow();
    
    Begin("Scene tree" );
    SetWindowPos( ImVec2(0,0) );
    SetWindowSize( ImVec2( 640 ,0) );
    
    
    if(Button("New node")){
        if( selected_node ) selected_node->add_child( new SceneNode() );
        else if(root_node ) root_node->add_child( new SceneNode() );
        else Engine::get_current_scene()->set_root_node(new SceneNode());  
    }
    SameLine();    
    if(Button("Delete node") && selected_node) selected_node->queue_free();
    
    Columns(2,"scene_tree_and_inspector",true);
    SetColumnWidth(0,250);
    if( root_node ){
        SetNextTreeNodeOpen(true);
        push_node_tree(root_node);
    }
    NextColumn();
    inspector();
    End();

    push_render_window();

    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

