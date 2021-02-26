cmake_minimum_required( VERSION 3.5 )

project(saucer_engine)

option( SAUCER_BUILD_EDITOR "Build editor. Only valid if build type is debug" ON )
option( SAUCER_LINK_COMPONENTS "Build editor. Only valid if build type is debug" ON )

# zlib
if( MSVC )
	add_library(zlibstatic STATIC IMPORTED)
	set_property(TARGET zlibstatic PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/deps/zlib/msvc2017_64/lib/zlib/zlibstatic.lib )
	target_include_directories( zlibstatic INTERFACE ${PROJECT_SOURCE_DIR}/deps/zlib/msvc2017_64/include/zlib )
	set( ZLIB_LIBRARY zlibstatic )
else()
	add_subdirectory("deps/zlib")
	list( APPEND LINK_LIBRARIES zlibstatic )
	set( ZLIB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/zlib) # idk why but still needed by Mingw 
endif()

# AudioFile (header only)
list( APPEND INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/deps/audiofile )

# OGG
add_subdirectory("deps/ogg")
list( APPEND LINK_LIBRARIES Ogg::ogg )

# Vorbis
get_target_property( OGG_INCLUDE_DIR Ogg::ogg INTERFACE_INCLUDE_DIRECTORIES )
set( OGG_LIBRARY Ogg::ogg )
add_subdirectory("deps/vorbis")
list( APPEND LINK_LIBRARIES vorbisfile )

# yaml-cpp
add_subdirectory("deps/yaml-cpp")
list( APPEND LINK_LIBRARIES yaml-cpp )

# lua
add_subdirectory("deps/lua")
list( APPEND LINK_LIBRARIES lua )

# glfw
add_subdirectory("deps/glfw")
list( APPEND LINK_LIBRARIES glfw )

# glew
option( ONLY_LIBS "" ON )
add_subdirectory("deps/glew")
#set(OPENGL_opengl_LIBRARY GL CACHE STRING "" FORCE ) # Overriding this from cmake find module 
list( APPEND LINK_LIBRARIES libglew_static )

# openal-soft
set( LIBTYPE STATIC )
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
option(PNG_STATIC "Build static lib" ON)
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
list( APPEND IMGUI_INCLUDE_DIRECTORIES 
		${PROJECT_SOURCE_DIR}/deps/imgui 
		${PROJECT_SOURCE_DIR}/deps/imgui/examples 
		${PROJECT_SOURCE_DIR}/deps/imgui/misc/cpp )
target_include_directories( imgui PUBLIC ${IMGUI_INCLUDE_DIRECTORIES} )
target_link_libraries( imgui PUBLIC glfw libglew_static )
list( APPEND INCLUDE_DIRECTORIES ${IMGUI_INCLUDE_DIRECTORIES} )
list( APPEND LINK_LIBRARIES $<$<AND:$<BOOL:SAUCER_BUILD_EDITOR>,$<CONFIG:Debug>>:imgui> )

set( CMAKE_CXX_STANDARD 11)

# Linking everything into saucer_core lib 
file( GLOB_RECURSE CORE_ENGINE_SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp )
list( APPEND ENGINE_SRCS ${CORE_ENGINE_SRCS} )
list( APPEND INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/src )

add_library( saucer_core STATIC ${ENGINE_SRCS} )
target_compile_definitions( saucer_core PUBLIC GLEW_STATIC 
	$<$<CONFIG:Debug>: DEBUG >
	$<$<AND:$<BOOL:SAUCER_BUILD_EDITOR>,$<CONFIG:Debug>>:SAUCER_EDITOR> 
)
target_include_directories( saucer_core PUBLIC ${INCLUDE_DIRECTORIES} )
target_link_libraries( saucer_core PUBLIC ${LINK_LIBRARIES} ${COMPONENT_LIBRARIES} )

file( GLOB COMPONENT_PATHS LIST_DIRECTORIES true ${PROJECT_SOURCE_DIR}/components/* )

foreach( COMPONENT_PATH ${COMPONENT_PATHS} )
	add_subdirectory( ${COMPONENT_PATH} )
	get_filename_component( COMPONENT ${COMPONENT_PATH} NAME )
	message("Found component module: " ${COMPONENT})
	list( APPEND COMPONENT_LIBRARIES ${COMPONENT} )
endforeach()

add_executable( saucer ${PROJECT_SOURCE_DIR}/main.cpp )
target_link_libraries( saucer PUBLIC saucer_core $<$<BOOL:${SAUCER_LINK_COMPONENTS}>:${COMPONENT_LIBRARIES}> )
if( MSVC )
	list( APPEND SAUCER_LINK_OPTIONS $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS> )
endif()
target_link_options( saucer PUBLIC ${SAUCER_LINK_OPTIONS}  )
