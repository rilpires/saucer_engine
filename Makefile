
LINKER_FOLDERS := 	\
	obj \
	deps/lua/lib \
	deps/libpng/lib \
	deps/glfw/src \
	deps/glew/lib \
	deps/openal_soft \
	deps/box2d/build/bin \
	deps/freetype/build  \
	deps/yaml-cpp/build

LIBS := \
	png \
	glfw3 \
	pthread \
	dl \
	GL \
	GLEW \
	openal common ex-common \
	lua lualib \
	yaml-cpp \
	freetype \
	box2d 

INCLUDE_PATHS := \
	include \
	deps/lua/include \
	deps/libpng/include \
	deps/glfw/include \
	deps/glew/include \
	deps/audiofile \
	deps/freetype/include \
	deps/openal_soft/include \
	deps/yaml-cpp/include \
	deps/imgui deps/imgui/examples deps/imgui/misc/cpp \
	deps/box2d/include

RELEASE ?= 0

CXX	      := g++
CXX_FLAGS := -std=c++11 -Wall -Wextra
SRC_FILES := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(SRC_FILES) )
EXEC_NAME := main

ifeq ($(RELEASE),0)
	CXX_FLAGS += -g -DDEBUG -DSAUCER_EDITOR
	EXEC_NAME := $(EXEC_NAME)_debug	
	OBJ_FILES := $(patsubst %.o, %.debug.o, $(OBJ_FILES) )
	
	# Editor dependencies
	OBJ_FILES +=  	obj/imgui.o \
				 	obj/imgui_draw.o \
				 	obj/imgui_widgets.o \
				 	obj/imgui_demo.o \
				 	obj/imgui_impl_glfw.o \
				 	obj/imgui_stdlib.o \
				 	obj/imgui_impl_opengl3.o

else
	CXX_FLAGS += -O3
	EXEC_NAME := $(EXEC_NAME)_release
	OBJ_FILES := $(patsubst %.o, %.release.o, $(OBJ_FILES) )
	OBJ_FILES := $(filter-out obj/editor.release.o, $(OBJ_FILES) )
endif

main_release main_debug : $(OBJ_FILES)
	@echo Linking everything
	$(CXX) $(CXX_FLAGS) $(OBJ_FILES) -Bstatic -o $(EXEC_NAME) $(patsubst %, -L%, $(LINKER_FOLDERS) ) $(patsubst %, -l%, $(LIBS) ) $(patsubst %, -I%, $(INCLUDE_PATHS))

obj/imgui_stdlib.o : deps/imgui/misc/cpp/imgui_stdlib.cpp
	@echo Compiling DearImGui implementation $@
	$(CXX) $(CXX_FLAGS) $< -c -o $@  $(patsubst %, -I%, $(INCLUDE_PATHS))
obj/imgui_impl_%.o : deps/imgui/examples/imgui_impl_%.cpp
	@echo Compiling DearImGui implementation $@
	$(CXX) $(CXX_FLAGS) $< -c -o $@  $(patsubst %, -I%, $(INCLUDE_PATHS))
obj/imgu%.o : deps/imgui/imgu%.cpp
	@echo Compiling DearImGui implementation $@
	$(CXX) $(CXX_FLAGS) $< -c -o $@  $(patsubst %, -I%, $(INCLUDE_PATHS))

obj/%.release.o obj/%.debug.o : src/%.cpp
	@echo Compiling $@
	$(CXX) $(CXX_FLAGS) $< -c -o $@  $(patsubst %, -I%, $(INCLUDE_PATHS)) 

clean:
	@rm -f $(OBJ_FILES) $(EXEC_NAME)