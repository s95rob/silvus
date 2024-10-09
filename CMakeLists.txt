cmake_minimum_required(VERSION 3.10)

project(silvus)

add_subdirectory("extern/glfw")
add_subdirectory("extern/glad")

file(GLOB_RECURSE SILVUS_SRCS "src/*" "include/*")
add_executable(silvus ${SILVUS_SRCS})
target_include_directories(silvus 
PUBLIC
	"include"
	"extern/header-only"
)
target_link_libraries(silvus
PUBLIC
	glad
	glfw
)