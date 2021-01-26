#include "editor.h"
#include "core.h"
#include <fstream>
#include <algorithm>

using namespace ImGui;

SaucerId        SaucerEditor::node_id_selected = 0;
bool            SaucerEditor::currently_playing = false;
SceneNode*      SaucerEditor::tree_root_before_play = nullptr;
std::string     SaucerEditor::current_scene_path = "";
SaucerObject*   SaucerEditor::object_to_be_saved = nullptr;

SaucerEditor::EditorStream    SaucerEditor::stream;
std::unordered_map<SaucerId,std::string> SaucerEditor::reference_path;


int SaucerEditor::EditorStream::overflow (int c){
    str += (char)c;
    return c;
}

const char* SaucerEditor::EditorStream::get_buffer() const {
    return str.c_str();
}
size_t      SaucerEditor::EditorStream::get_size() const {
    return str.size();
}
void      SaucerEditor::EditorStream::clear() {
    str.clear();
}


void            SaucerEditor::setup(){
    
    IMGUI_CHECKVERSION();
    CreateContext();    
    extern_console_streams.push_back(&SaucerEditor::stream);

    ImGuiIO& io = GetIO(); (void)io;
    StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL( Engine::get_render_engine()->get_glfw_window(), true );
    ImGui_ImplOpenGL3_Init("#version 150");
}


void            SaucerEditor::push_scene_tree_window(){
    SceneNode* root_node = Engine::get_current_scene()->get_root_node();
    SceneNode* selected_node = (SceneNode*)SaucerObject::from_saucer_id(SaucerEditor::node_id_selected);

    Begin("Scene tree" );
    
    if( BeginPopup("save_object") ){
        SAUCER_ASSERT(object_to_be_saved!=nullptr, "object_to_be_saved should not be null when BeginPopup(\"save_object\") is called!");
        std::string p;
        if( object_to_be_saved==root_node && current_scene_path.size() ) p = current_scene_path;
        bool save_confirmed = InputText("",&p,ImGuiInputTextFlags_EnterReturnsTrue);
        SameLine();
        save_confirmed = save_confirmed || Button("Save") ;
        if( save_confirmed ){
            std::ofstream ofs;
            ofs.open( p , std::ofstream::out );
            ofs << object_to_be_saved->to_yaml_node();
            ofs.close();
            object_to_be_saved = nullptr;
            CloseCurrentPopup();
        }
        EndPopup();
    }
    if( BeginPopup("open_scene") ){
        static std::string popup_open_scene_path;
        bool path_confirmed = InputText("",&popup_open_scene_path,ImGuiInputTextFlags_EnterReturnsTrue);
        SameLine();
        path_confirmed = path_confirmed || Button("Open") ;
        if( path_confirmed ){
            root_node->get_out();
            root_node->queue_free();
            root_node = new SceneNode();
            root_node->SaucerObject::from_yaml_node( popup_open_scene_path );
            Engine::get_current_scene()->set_root_node(root_node);
            current_scene_path = popup_open_scene_path;
            CloseCurrentPopup();
        }
        EndPopup();
    }
    if( BeginPopup("open_scene_as_child") ){
        static std::string open_scene_as_child_path;
        bool path_confirmed = InputText("",&open_scene_as_child_path,ImGuiInputTextFlags_EnterReturnsTrue);
        SameLine();
        path_confirmed = path_confirmed || Button("Open") ;
        if( path_confirmed ){
            if( open_scene_as_child_path == current_scene_path ){
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

    if(Button("Open node tree") && root_node ){
        OpenPopup("open_scene");
    }
    SameLine();    
    if(Button("Save node tree") && root_node ){
        object_to_be_saved = root_node;
        OpenPopup("save_object");
    }
    SameLine();
    if(Button("Save selected node") && selected_node ){
        object_to_be_saved = selected_node;
        OpenPopup("save_object");
    }
    SameLine();
    if(Button("New node")){
        if( selected_node && get_reference_path(selected_node)==nullptr ) selected_node->add_child( new SceneNode() );
        else if(root_node ) root_node->add_child( new SceneNode() );
        else Engine::get_current_scene()->set_root_node(new SceneNode());  
    }
    SameLine();
    if(Button("Instatiate from file") ){
        OpenPopup("open_scene_as_child");
    }
    
    Columns(2,"scene_tree_and_inspector",true);
    //SetColumnWidth(0, GetWindowSize().x * 0.45 );
    if( root_node ){
        SetNextTreeNodeOpen(true);
        push_node_tree(root_node);
    }
    NextColumn();
    push_inspector();
    clamp_window_on_screen();
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
        SameLine();
        PROPERTY_BOOL(selected_node,inherits_transform);
        
        std::string lua_script_path;
        if( selected_node->get_script() ) lua_script_path = selected_node->get_script()->get_path();
        if( InputText( "Lua script path" , &lua_script_path , ImGuiInputTextFlags_EnterReturnsTrue ) ){
            selected_node->set_script( ResourceManager::get_resource<LuaScriptResource>(lua_script_path) );
        }
        
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
    
    clamp_window_on_screen();

    End();
}
void            SaucerEditor::push_node_tree( SceneNode* node ){

    bool is_opened = TreeNodeEx(    (void*)(long long)node->get_saucer_id(),
                            ImGuiTreeNodeFlags_OpenOnArrow 
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
        }
        EndDragDropTarget();
    }
    if( IsItemClicked() ) SaucerEditor::node_id_selected = node->get_saucer_id();
    
    
    if( get_reference_path(node)==nullptr ) {
        SameLine(); SmallButton("+"); if( IsItemClicked() ) node->add_child(new SceneNode());
    } else {
        SameLine(); TextColored( ImVec4(1.0,0.5,0.5,1.0) , "[Referenced]" );
    }
    SameLine(); SmallButton("-"); if( IsItemClicked() ) node->queue_free();
    
    if( is_opened ){
        std::string* ref_path = get_reference_path(node);
        if( ref_path==nullptr )
            for( auto child : node->get_children() ) 
                push_node_tree(child);
        TreePop();
    }
}
void            SaucerEditor::push_render_window(){

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
    if( !currently_playing && MenuItem("Play current scene") ){
        tree_root_before_play = Engine::get_current_scene()->get_root_node();
        Engine::get_current_scene()->set_root_node( nullptr );
        node_id_selected = 0;
        currently_playing = true;
        ResourceManager::dirty_every_resource();
        Engine::get_current_scene()->set_root_node(tree_root_before_play->duplicate());
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
    clamp_window_on_screen();
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
std::string*    SaucerEditor::get_reference_path( const SceneNode* n ){
    auto it = reference_path.find(n->get_saucer_id());
    if( it != reference_path.end() ){
        if( it->second == current_scene_path ) return nullptr;
        return &(it->second);
    } else return nullptr;
}
void            SaucerEditor::flag_as_referenced( const SceneNode* n , std::string path ){
    reference_path[n->get_saucer_id()] = path;
}
void            SaucerEditor::clamp_window_on_screen(){
    if( GetWindowPos().x < 0 ) SetWindowPos( ImVec2( 0 , GetWindowPos().y ) );
    if( GetWindowPos().y < 0 ) SetWindowPos( ImVec2( GetWindowPos().x , 0 ) );
    if( GetWindowPos().x + GetWindowSize().x > Engine::get_window_size().x ) 
        SetWindowPos( ImVec2( Engine::get_window_size().x - GetWindowSize().x , GetWindowPos().y ) );
    if( GetWindowPos().y + GetWindowSize().y > Engine::get_window_size().y ) 
        SetWindowPos( ImVec2( GetWindowPos().x , Engine::get_window_size().y - GetWindowSize().y ) );
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
    // ==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--

    

    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

