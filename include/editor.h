#ifndef SAUCER_EDITOR_H
#define SAUCER_EDITOR_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include "saucer_object.h"

namespace SaucerEditor {

    extern SaucerId node_id_selected;
    
    void setup();
    void update();
    
};

#endif