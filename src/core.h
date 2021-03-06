#ifndef CORE_H
#define CORE_H


#ifdef _WIN32
	#include <chrono>
	#include <thread>
#else
	#include <unistd.h>
#endif

// Basic data structures
#include "color.h"
#include "vector.h"
#include "rect.h"

#include "saucer_object.h"
#include "saucer_object.h"
#include "engine.h"
#include "lua_engine.h"
#include "audio.h"
#include "component.h"
#include "label_rect.h"
#include "sprite.h"
#include "transform.h"
#include "collision.h"
#include "render_engine.h"
#include "resources.h"
#include "camera.h"
#include "patch_rect.h"
#include "resources/audiofile.h"
#include "input.h"
#include "debug.h"
#include "scene.h"
#include "scene_node.h"
#include "file.h"
#include "editor.h"

#include "resources/project_config.h"
#include "resources/node_template.h"
#include "resources/font.h"
#include "resources/image.h"
#include "resources/shader.h"
#include "resources/lua_script.h"

#endif