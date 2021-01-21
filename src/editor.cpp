#include "editor.h"
#include "core.h"

namespace SaucerEditor {

void setup(){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL( Engine::get_render_engine()->get_glfw_window(), true );
    ImGui_ImplOpenGL3_Init("#version 150");
}

void update(){

    SceneNode* root_node = Engine::get_current_scene()->get_root_node();
    if( !root_node ) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
    
    ImGui::Begin("Scene tree" );   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

    Color::ColorFloat c = root_node->get_modulate().to_float();
    if( ImGui::ColorEdit4("Modulate", (float*)&c , 0 ) ) root_node->set_modulate(c.to_color());
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


}