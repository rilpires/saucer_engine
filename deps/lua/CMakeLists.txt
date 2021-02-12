
cmake_minimum_required( VERSION 3.5 )

project(lua)


file(GLOB_RECURSE LUA_SRC_FILES "src/*.c")

add_library( lua STATIC ${LUA_SRC_FILES} )

target_include_directories( lua PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${PROJECT_SOURCE_DIR}/src )