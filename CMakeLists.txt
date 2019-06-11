cmake_minimum_required(VERSION 3.13)

project (ImFileDialogDemo)

find_package(OpenGL REQUIRED)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw/)

file(GLOB demo_src CONFIGURE_DEPENDS *.cpp *.h *.hpp file_dialog/*.cpp file_dialog/*.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/dirent/include/ ${CMAKE_CURRENT_SOURCE_DIR}/file_dialog ${CMAKE_CURRENT_SOURCE_DIR}/libs/gl3w/)

add_executable(demo ${demo_src} ./libs/gl3w/GL/gl3w.c)

target_link_libraries(demo glfw OpenGL::GL)


