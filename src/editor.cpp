#include "editor.h"
#include "core.h"
#include <fstream>
#include <algorithm>

using namespace ImGui;

SaucerId        SaucerEditor::node_id_selected = 0;
bool            SaucerEditor::currently_playing = false;
SceneNode*      SaucerEditor::tree_root_before_play = nullptr;

SaucerEditor::EditorStream    SaucerEditor::stream;
std::unordered_map<SaucerId,std::string> SaucerEditor::reference_path;
std::unordered_map<std::string, std::vector<std::string> > SaucerEditor::resources_by_type;

int             SaucerEditor::EditorStream::overflow (int c){
    str += (char)c;
    return c;
}
const char*     SaucerEditor::EditorStream::get_buffer() const {
    return str.c_str();
}
size_t          SaucerEditor::EditorStream::get_size() const {
    return str.size();
}
void            SaucerEditor::EditorStream::clear() {
    str.clear();
}

void            SaucerEditor::setup(){
    
    IMGUI_CHECKVERSION();
    CreateContext();    
    extern_console_streams().push_back(&SaucerEditor::stream);

    ImGuiIO& io = GetIO(); (void)io;
    StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL( Engine::get_render_engine()->get_glfw_window(), true );
    ImGui_ImplOpenGL3_Init("#version 130");
}

void            SaucerEditor::push_scene_tree_window(){
    SceneNode* root_node = Engine::get_current_scene()->get_root_node();
    SceneNode* selected_node = (SceneNode*)SaucerObject::from_saucer_id(SaucerEditor::node_id_selected);
    static SceneNode* object_to_be_saved = nullptr;
    static std::string object_to_be_saved_path;
    Begin("Scene tree" );
    

    if(Button("Open node tree") && root_node ){
        OpenPopup("open_scene");
    }
    SameLine();    
    if(Button("Save node tree") && root_node ){
        object_to_be_saved = root_node;
        OpenPopup("save_object");
    }
    SameLine();
    if(Button("Save selected node") && selected_node && (selected_node!=root_node) ){
        object_to_be_saved = selected_node;
        OpenPopup("save_object");
    }
    SameLine();
    if(Button("New node")){
        if( selected_node && get_reference_path(selected_node).empty() ) selected_node->add_child( new SceneNode() );
        else if(root_node ) root_node->add_child( new SceneNode() );
        else Engine::get_current_scene()->set_root_node(new SceneNode());  
    }
    SameLine();
    if(Button("Instatiate from file") ){
        OpenPopup("open_scene_as_child");
    }
    
    Columns(2,"scene_tree_and_inspector",true);
    if( root_node ){
        SetNextTreeNodeOpen(true);
        push_node_tree(root_node);
    }
    NextColumn();
    push_inspector();
    

    if( BeginPopup("open_scene") ){
        static std::string popup_open_scene_path;
        if( push_resource_hint( "NodeTemplateResource" , popup_open_scene_path ) ){
            NodeTemplateResource* res = ResourceManager::get_resource<NodeTemplateResource>(popup_open_scene_path);
            if(!res){
                saucer_err(popup_open_scene_path , " is a invalid file path.");
            } else {
                root_node->get_out();
                root_node->queue_free();
                root_node = new SceneNode();
                res->flag_as_dirty();
                root_node->SaucerObject::from_yaml_node( popup_open_scene_path );
                Engine::get_current_scene()->set_root_node(root_node);
                reference_path[root_node->get_saucer_id()] = popup_open_scene_path;
                CloseCurrentPopup();
            }
        }
        EndPopup();
    }
    if( BeginPopup("open_scene_as_child") ){
        static std::string open_scene_as_child_path;

        if( push_resource_hint( "NodeTemplateResource" , open_scene_as_child_path ) ){
            if( open_scene_as_child_path == get_reference_path(root_node) ){
                saucer_warn("You can't put a parent node as a child of one of it's childs!" );
            } else if (!selected_node){
                saucer_warn("You have to select a node to instantiate a child.");
            } else {
                SceneNode* child_node = new SceneNode();
                child_node->SaucerObject::from_yaml_node( open_scene_as_child_path );
                selected_node->add_child(child_node);
                reference_path[child_node->get_saucer_id()] = open_scene_as_child_path;
                CloseCurrentPopup();
            }
        }
        EndPopup();
    }
    
    DefinePopupSaveNode(object_to_be_saved ,object_to_be_saved_path);

    End(); // SceneTree
}
void            SaucerEditor::push_inspector(){
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
        SameLine();
        PROPERTY_BOOL(selected_node,visible);
        PROPERTY_RESOURCE(selected_node,script,LuaScriptResource);
        
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
        
        if( Button("Create component")){
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
    
        if( currently_playing ){
            TextColored( ImVec4(0.8,1.0,0.8,1.0) , "Lua script variables:");
            static bool inspector_filter_out_functions = true;
            Checkbox("filter_out_functions",&inspector_filter_out_functions);
            LuaEngine::push_actor_items(selected_node,inspector_filter_out_functions);
        }
    }
}
void            SaucerEditor::push_config(){
    YamlNode& config = Engine::get_config();

    Begin("Project configuration" , 0 , ImGuiWindowFlags_MenuBar );
    if(BeginMenuBar()){
        if(MenuItem("Save")){
            std::ofstream ofs("config.yaml",std::ofstream::out);
            ofs << config;
            ofs.close();
        }
        EndMenuBar();
    }
    
    Vector2 initial_window_size = Engine::get_config()["initial_window_size"].as<Vector2>();
    if( InputFloat2( "initial_window_size" , (float*)&initial_window_size , "%.0f" ) )
        config["initial_window_size"] = initial_window_size;    
    
    End();
}
void            SaucerEditor::push_node_tree( SceneNode* node ){

    bool is_reference = !get_reference_path(node).empty() && (Engine::get_current_scene()->get_root_node() != node) ;
    bool is_leaf = (node->get_children().size()==0);
    bool is_opened; 

    is_opened = TreeNodeEx(    (void*)(long long)node->get_saucer_id(),
                            ImGuiTreeNodeFlags_OpenOnArrow 
                            + ImGuiTreeNodeFlags_Bullet*is_leaf
                            + ImGuiTreeNodeFlags_Selected*(node->get_saucer_id()==SaucerEditor::node_id_selected),
                            "%s" , node->get_name().c_str() );
    
    
    if( BeginDragDropSource( ImGuiDragDropFlags_SourceNoDisableHover ) ){
        SaucerId node_id = node->get_saucer_id();
        SetDragDropPayload("SCENE_NODE",(void*)&node_id,sizeof(SaucerId));
        EndDragDropSource();
    }
    if( BeginDragDropTarget() ){
        const ImGuiPayload* payload = AcceptDragDropPayload("SCENE_NODE");
        if( payload ){
            SaucerId* received_saucer_id = static_cast<SaucerId*>(payload->Data);
            SceneNode* received_node = (SceneNode*)SaucerObject::from_saucer_id(*received_saucer_id);
            if( received_node->is_parent_of(node) ){
                saucer_warn("You can't set a node as a child of it's childs you know");
            } else {
                received_node->get_out();
                node->add_child(received_node);
            }
        } else {
            payload = AcceptDragDropPayload("resource_path");
            if( payload ){
                std::string& res_path = *static_cast<std::string*>(payload->Data);
                size_t pos = res_path.find(".node");
                if( pos == std::string::npos ){
                    saucer_warn("This doesn't looks like a node template resource... (.node)");
                } else {
                    SceneNode* new_child = ResourceManager::get_resource<NodeTemplateResource>(res_path)->instantiate_node();
                    reference_path[new_child->get_saucer_id()] = res_path;
                    node->add_child( new_child );
                }
            }
        }
        EndDragDropTarget();
    }
    if( IsItemClicked() ) SaucerEditor::node_id_selected = node->get_saucer_id();
    
    
    if( !is_reference ) {
        SameLine(); SmallButton("+"); if( IsItemClicked() ) node->add_child(new SceneNode());
    } else {
        SameLine(); TextColored( ImVec4(1.0,0.5,0.5,1.0) , "[Referenced]" );
        SameLine(); SmallButton("Unref"); if( IsItemClicked() ) reference_path.erase(node->get_saucer_id());
    }
    SameLine(); SmallButton("-"); if( IsItemClicked() ) node->queue_free();
    if( node->get_parent() ){
        SameLine(); SmallButton(".."); if( IsItemClicked() ){
            SceneNode* copy = node->duplicate();
            std::string reference_path = get_reference_path(node);
            if( !reference_path.empty() ) flag_as_referenced(copy,reference_path);
            node->get_parent()->add_child( copy );   
        }
    }

    if( is_opened ){
        if( is_reference==false )
            for( auto child : node->get_children() ) 
                push_node_tree(child);
        TreePop();
    }
}
void            SaucerEditor::push_render_window(){

    SetNextWindowBgAlpha(0);
    SceneNode* root_node = Engine::get_current_scene()->get_root_node();
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
    if( root_node && !currently_playing && MenuItem("Play current scene") ){
        tree_root_before_play = root_node;
        std::string current_tree_root_path = get_reference_path(root_node);
        Engine::get_current_scene()->set_root_node( nullptr );
        node_id_selected = 0;
        currently_playing = true;
        ResourceManager::dirty_every_resource();
        LuaEngine::reset();
        stream.clear();
        if( !current_tree_root_path.empty() ) reference_path.erase(root_node->get_saucer_id()); // We don't want the new duplicated root to know that it is referenced, so that's why we are temporarily disabling it
        Engine::get_current_scene()->set_root_node( tree_root_before_play->duplicate() );
        if( !current_tree_root_path.empty() ) reference_path[tree_root_before_play->get_saucer_id()] = current_tree_root_path; 
    }
    if( currently_playing && MenuItem("Stop playing") ){
        SceneNode* root_while_played = Engine::get_current_scene()->get_root_node();
        root_while_played->queue_free();
        Engine::get_current_scene()->set_root_node(nullptr);
        currently_playing = false;
        Engine::get_current_scene()->set_root_node(tree_root_before_play);
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
void            SaucerEditor::push_lua_profiler(){
    Begin("Lua profiler");
    
    if( LuaEngine::get_kb_memory_used() < 1000 ) Text("Memory used:%d KB" , LuaEngine::get_kb_memory_used() );
    else Text("Memory used: %.2f MB" , float(LuaEngine::get_kb_memory_used())/1000 );
    if( LuaEngine::get_kb_memory_threshold() < 1000 ) Text("GC threshold:%d KB" , LuaEngine::get_kb_memory_threshold() );
    else Text("GC threshold: %.2f MB" , float(LuaEngine::get_kb_memory_threshold())/1000 );
    
    NewLine();
    if( TreeNodeEx("Environment:" , ImGuiTreeNodeFlags_Framed ) ){
        static bool only_show_actors = false;
        Checkbox("only_show_actors" , &only_show_actors);
        static bool filter_out_functions = false;
        Checkbox("filter_out_functions" , &filter_out_functions);
        NewLine();
        LuaEngine::push_editor_items( filter_out_functions , only_show_actors );
        TreePop();
    }

    End();
}
void            SaucerEditor::push_lua_editor(){
    Begin("Lua script editor [BETA]" , nullptr , ImGuiWindowFlags_MenuBar );
    std::string txt;
    const ImVec2 v = GetContentRegionAvail();
    if( BeginMenuBar() ){
        if( BeginMenu("File") ){
            if (MenuItem("Open")){
                saucer_print("hiii");
            }
            EndMenu();
        }
        EndMenuBar();
    }
    InputTextMultiline( "" , &txt , v );
    End();
}
void            SaucerEditor::push_engine_profiler(){
    Begin("Engine profiler");
    int window_content_width = GetContentRegionAvail().x;
    ImVec2 plot_sizes = ImVec2( window_content_width , 32 );

    static std::vector<float> last_fps;
    last_fps.push_back( Engine::get_fps() );
    if( last_fps.size() > 120 ) last_fps.erase( last_fps.begin() );
    auto min_max = std::minmax_element(last_fps.begin() , last_fps.end());
    float total_sum = 0.0f; for( auto v : last_fps ) total_sum += v;
    float mean = total_sum / last_fps.size();
    char buff[127];
    sprintf(buff,"[FPS] min:%.2f ; mean:%.2f ; max: %.2f" , *(min_max.first) , *(min_max.second) , mean );
    PlotLines("", &(last_fps[0]) , last_fps.size() , 0 , buff , 0 , 60 , plot_sizes );


    End();
}
void            SaucerEditor::push_console(){
    Begin("Console");
    
    if( Button("Clear") ) stream.clear();

    BeginChild("Text");
    static size_t last_size = 0;
    TextUnformatted( stream.get_buffer() );
    if( last_size != stream.get_size() )
        SetScrollY( GetScrollMaxY() );
    last_size = stream.get_size();
    EndChild();

    End();
}
void            SaucerEditor::push_resource_explorer(){
    Begin("Resource explorer");
    static std::string resource_explorer_filter;
    resources_by_type.clear();
    InputText( "Filter" , &resource_explorer_filter );
    for( auto it = ResourceManager::begin() ; it != ResourceManager::end() ; it++ ){
        if( resource_explorer_filter.size() == 0 || it->first.find( resource_explorer_filter ) != std::string::npos ){
            std::string class_name = SaucerObject::from_saucer_id(it->second)->my_saucer_class_name();
            resources_by_type[class_name].push_back( it->first );
        }
    }
    for( auto it : resources_by_type ){
        if( TreeNodeEx(it.first.c_str() , ImGuiTreeNodeFlags_Framed+ImGuiTreeNodeFlags_SpanFullWidth ) ){
            for( auto& res_path : it.second ){
                Selectable( res_path.c_str() );
                if( BeginDragDropSource( ImGuiDragDropFlags_SourceNoDisableHover ) ){
                    static std::string resource_path_payload = res_path;
                    SetDragDropPayload("resource_path" , &resource_path_payload , sizeof(std::string));
                    EndDragDropSource();
                }
            }
            TreePop();
        }
    }
    End();


}
std::string     SaucerEditor::get_reference_path( const SceneNode* n ){
    auto it = reference_path.find(n->get_saucer_id());
    if( it != reference_path.end() ){
        return it->second;
    } else return std::string();
}
void            SaucerEditor::flag_as_referenced( const SceneNode* n , std::string path ){
    reference_path[n->get_saucer_id()] = path;
}
bool            SaucerEditor::push_resource_hint( std::string resource_type , std::string& path ){
    bool ret = false;
    std::string input_string;
    if( InputText("",&input_string,ImGuiInputTextFlags_EnterReturnsTrue) ){
        path = input_string;
        ret = true;
    } else if( TreeNodeEx( resource_type.c_str() , ImGuiTreeNodeFlags_Framed + ImGuiTreeNodeFlags_DefaultOpen ) ){
        for( auto it : resources_by_type[resource_type] ){
            if( path.empty() || it.find(input_string) != std::string::npos ){
                if( Selectable(it.c_str())){
                    path = it;
                    ret = true;
                    break;
                }
            }
        }
        TreePop();
    }
    if( !path.empty() && ret ){
        Resource* res = ResourceManager::get_resource<Resource>( path );
        if(!res) saucer_err("Resource with path \"",path,"\" is not valid");
    }
    return ret;
}
void            SaucerEditor::update(){
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();
    
    // ==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--
    ShowDemoWindow();           
    push_scene_tree_window();
    push_render_window();
    push_config();
    push_engine_profiler();
    push_lua_profiler();
    push_lua_editor();
    push_console();
    push_resource_explorer();
    // ==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--

    // Shortcuts .-.-.-.-.-.-.-.-.-.-.-.-
    if( ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) ){
        if( ImGui::IsKeyPressed(GLFW_KEY_S,false) ){
            OpenPopup("save_object");
        }
    }
    std::string save_path;
    DefinePopupSaveNode( Engine::get_current_scene()->get_root_node() , save_path );

    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}
void            SaucerEditor::DefinePopupSaveNode( SceneNode* node , std::string& path ){
    if(!node)return;
    if( BeginPopup("save_object") ){
        std::string ref_path = get_reference_path(node);
        if( !ref_path.empty() ) path = ref_path;
        bool save_confirmed = InputText("",&path,ImGuiInputTextFlags_EnterReturnsTrue);
        SameLine();
        save_confirmed = save_confirmed || Button("Save tree") ;
        if( save_confirmed && path.size() ){
            
            if(!ref_path.empty()) reference_path.erase(node->get_saucer_id());
            node->save_as_file(path);
            reference_path[node->get_saucer_id()] = path;
            
            ResourceManager::get_resource<NodeTemplateResource>(path)->flag_as_dirty();
            CloseCurrentPopup();
        }
        EndPopup();
    }
}