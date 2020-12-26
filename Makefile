
LINKER_FOLDERS := 	\
	obj \
	deps/lua/lib \
	deps/libpng/lib \
	deps/glfw/src \
	deps/glew/lib 

LIBS := \
	png \
	glfw3 \
	pthread \
	dl \
	GL \
	GLEW \
	lua lualib\

INCLUDE_PATHS := \
	include \
	deps/lua/include \
	deps/libpng/include \
	deps/glfw/include \
	deps/glew/include

RELEASE ?= 0

CXX	      := g++
CXX_FLAGS := -std=c++11 -Wall
SRC_FILES := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(SRC_FILES) )
EXEC_NAME := main

ifeq ($(RELEASE),0)
	CXX_FLAGS += -g -DDEBUG
	EXEC_NAME := $(EXEC_NAME)_debug
	OBJ_FILES := $(patsubst %.o, %.debug.o, $(OBJ_FILES) )
else
	EXEC_NAME := $(EXEC_NAME)_release
	OBJ_FILES := $(patsubst %.o, %.release.o, $(OBJ_FILES) )
endif

main_release main_debug : $(OBJ_FILES)
	@echo Linking everything
	$(CXX) $(CXX_FLAGS) $(OBJ_FILES) -Bstatic -o $(EXEC_NAME) $(patsubst %, -L%, $(LINKER_FOLDERS) ) $(patsubst %, -l%, $(LIBS) ) $(patsubst %, -I%, $(INCLUDE_PATHS))

obj/%.release.o obj/%.debug.o : src/%.cpp
	@echo Compiling $@
	$(CXX) $(CXX_FLAGS) $< -c -o $@  $(patsubst %, -I%, $(INCLUDE_PATHS)) 

clean:
	@rm -f $(OBJ_FILES) $(EXEC_NAME)