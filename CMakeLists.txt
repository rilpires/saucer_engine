cmake_minimum_required( VERSION 3.5 )

project(saucer_engine)

set( PROJECT_TARGET saucer )


# zlib
set(PNG_BUILD_ZLIB ON)
set( ZLIB_LIBRARY zlibstatic )
add_library(zlibstatic STATIC IMPORTED)
set_property(TARGET zlibstatic PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/deps/zlib/msvc2017_64/lib/zlib/zlibstatic.lib )
set(ZLIB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/zlib/msvc2017_64/include/zlib)

# AudioFile (header only)
list( APPEND INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/deps/audiofile )


# yaml-cpp
set(YAML_CPP_BUILD_TESTS OFF)
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_CPP_BUILD_CONTRIB OFF)
set(YAML_CPP_INSTALL OFF)
add_subdirectory("deps/yaml-cpp")
list( APPEND LINK_LIBRARIES yaml-cpp )

# lua
add_subdirectory("deps/lua")
list( APPEND LINK_LIBRARIES lua )

# glfw
add_subdirectory("deps/glfw")
list( APPEND LINK_LIBRARIES glfw )

# glew
add_subdirectory("deps/glew")
list( APPEND LINK_LIBRARIES libglew_static )

# openal-soft
set( LIBTYPE STATIC )
add_subdirectory("deps/openal-soft")
list( APPEND LINK_LIBRARIES OpenAL common ex-common )

# freetype
add_subdirectory("deps/freetype")
list( APPEND LINK_LIBRARIES freetype )

# box2d
add_subdirectory("deps/box2d")
list( APPEND LINK_LIBRARIES box2d )

# libpng 
add_subdirectory("deps/libpng")
list( APPEND LINK_LIBRARIES png_static )
list( APPEND INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/deps/libpng ) # I don't know why but Cmake for libpng aparently do not properly propagate include dir


# ImGui. Depends on GLFW and GLEW
add_library( imgui OBJECT 
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
list(	APPEND LINK_LIBRARIES imgui )


set( CMAKE_CXX_STANDARD 11)

# Linking everything - directly linking from cpps
file(GLOB_RECURSE CORE_ENGINE_SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp )
list( APPEND INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include)

add_executable( ${PROJECT_TARGET} ${PROJECT_SOURCE_DIR}/main.cpp ${CORE_ENGINE_SRCS} )
target_compile_definitions( ${PROJECT_TARGET} PUBLIC SAUCER_EDITOR DEBUG )
target_include_directories( ${PROJECT_TARGET} PUBLIC ${INCLUDE_DIRECTORIES} )
target_link_libraries( ${PROJECT_TARGET} PUBLIC ${LINK_LIBRARIES} )
