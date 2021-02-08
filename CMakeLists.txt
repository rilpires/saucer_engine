cmake_minimum_required( VERSION 3.5 )

project(saucer_engine)


# zlib
if( MSVC )
	add_library(zlibstatic STATIC IMPORTED)
	set_property(TARGET zlibstatic PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/deps/zlib/msvc2017_64/lib/zlib/zlibstatic.lib )
	set( ZLIB_LIBRARY zlibstatic )
	set( ZLIB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/zlib/msvc2017_64/include/zlib)
else()
	add_subdirectory("deps/zlib")
	list( APPEND LINK_LIBRARIES zlibstatic )
	set( ZLIB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/zlib) # idk why but still needed by Mingw 
endif()

# AudioFile (header only)
list( APPEND INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/deps/audiofile )


# yaml-cpp
option(YAML_CPP_BUILD_TESTS "" OFF)
option(YAML_CPP_BUILD_TOOLS "" OFF)
option(YAML_CPP_BUILD_CONTRIB "" OFF)
option(YAML_CPP_INSTALL "" OFF)
add_subdirectory("deps/yaml-cpp")
list( APPEND LINK_LIBRARIES yaml-cpp )

# lua
add_subdirectory("deps/lua")
list( APPEND LINK_LIBRARIES lua )

# glfw
add_subdirectory("deps/glfw")
list( APPEND LINK_LIBRARIES glfw )

# glew
option( glew-cmake_BUILD_SHARED "" OFF )
option( ONLY_LIBS "" ON )
add_subdirectory("deps/glew")
set(OPENGL_opengl_LIBRARY GL CACHE STRING "" FORCE ) # Overriding this from cmake find module 
list( APPEND LINK_LIBRARIES libglew_static )

# openal-soft
set( LIBTYPE STATIC )
option( ALSOFT_EXAMPLES "" OFF )
option( ALSOFT_INSTALL 	"" OFF )
option( ALSOFT_UTILS 	"" OFF )
add_subdirectory("deps/openal-soft")
list( APPEND LINK_LIBRARIES OpenAL common ex-common )

# freetype
add_subdirectory("deps/freetype")
list( APPEND LINK_LIBRARIES freetype )

# box2d
option( BOX2D_BUILD_TESTBED "" OFF )
add_subdirectory("deps/box2d")
list( APPEND LINK_LIBRARIES box2d )

# libpng 
option(PNG_SHARED "Build shared lib" OFF)
option(PNG_STATIC "Build static lib" ON)
option(PNG_TESTS  "Build libpng tests" OFF)
option(PNG_BUILD_ZLIB "Custom zlib Location, else find_package is used" ON )
add_subdirectory("deps/libpng")
list( APPEND LINK_LIBRARIES png_static )
list( APPEND INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/deps/libpng ) # I don't know why but Cmake for libpng aparently do not properly propagate include dir


# ImGui. Depends on GLFW and GLEW
add_library( imgui STATIC 
		${PROJECT_SOURCE_DIR}/deps/imgui/imgui.cpp
		${PROJECT_SOURCE_DIR}/deps/imgui/imgui_draw.cpp
		${PROJECT_SOURCE_DIR}/deps/imgui/imgui_demo.cpp
		${PROJECT_SOURCE_DIR}/deps/imgui/imgui_widgets.cpp
		${PROJECT_SOURCE_DIR}/deps/imgui/examples/imgui_impl_opengl3.cpp
		${PROJECT_SOURCE_DIR}/deps/imgui/examples/imgui_impl_glfw.cpp
		${PROJECT_SOURCE_DIR}/deps/imgui/misc/cpp/imgui_stdlib.cpp)
target_include_directories( imgui PUBLIC 
		${PROJECT_SOURCE_DIR}/deps/imgui 
		${PROJECT_SOURCE_DIR}/deps/imgui/examples 
		${PROJECT_SOURCE_DIR}/deps/imgui/misc/cpp )
target_link_libraries( imgui PUBLIC glfw libglew_static )
list(	APPEND LINK_LIBRARIES $<$<CONFIG:Editor>:imgui> )

set( CMAKE_CXX_STANDARD 11)

# Linking everything - 
file(GLOB_RECURSE CORE_ENGINE_SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp )
list( APPEND INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include)


add_executable( saucer ${PROJECT_SOURCE_DIR}/main.cpp ${CORE_ENGINE_SRCS} )
target_compile_definitions( saucer PUBLIC GLEW_STATIC 
	$<$<CONFIG:Debug>: DEBUG >
	$<$<CONFIG:Editor>: DEBUG SAUCER_EDITOR > 
)
target_include_directories( saucer PUBLIC ${INCLUDE_DIRECTORIES} )
target_link_libraries( saucer PUBLIC ${LINK_LIBRARIES} )

