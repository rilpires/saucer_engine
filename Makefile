
LINKER_FOLDERS := 	\
	deps/libpng/lib \
	deps/glfw/src \
	deps/glew/lib 

LIBS := \
	png \
	glfw3 \
	pthread \
	dl \
	GL \
	GLEW

INCLUDE_PATHS := \
	deps/libpng/include \
	deps/glfw/include \
	deps/glew/include

DEBUG ?= 0

SRC_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(patsubst %.cpp, %.o, $(SRC_FILES) )
CXX_FLAGS := 
EXEC_NAME := main

ifeq ($(DEBUG),1)
	CXX_FLAGS += -g -DDEBUG
	EXEC_NAME := $(EXEC_NAME)_debug
	OBJ_FILES := $(patsubst %.o, %.debug.o, $(OBJ_FILES) )
else
	EXEC_NAME := $(EXEC_NAME)_release
	OBJ_FILES := $(patsubst %.o, %.release.o, $(OBJ_FILES) )
endif

main_release main_debug : $(OBJ_FILES)
	@echo Linking everything
	g++ $(OBJ_FILES) -Bstatic -o $(EXEC_NAME) $(patsubst %, -L%, $(LINKER_FOLDERS) ) $(patsubst %, -l%, $(LIBS) ) $(patsubst %, -I%, $(INCLUDE_PATHS)) $(CXX_FLAGS)

%.release.o %.debug.o : %.cpp
	@echo Compiling $@
	g++ $< -c -o $@  $(patsubst %, -I%, $(INCLUDE_PATHS)) $(CXX_FLAGS)

clean:
	@rm -f $(OBJ_FILES) $(EXEC_NAME)