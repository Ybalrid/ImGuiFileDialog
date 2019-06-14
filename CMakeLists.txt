cmake_minimum_required(VERSION 3.12)

project (ImFileDialogDemo)

find_package(OpenGL REQUIRED)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw/)

file(GLOB demo_src CONFIGURE_DEPENDS 
*.cpp 
*.h 
*.hpp 
file_dialog/*.cpp 
file_dialog/*.hpp 
imgui/*.cpp 
imgui/*.h 
imgui/examples/imgui_impl_opengl3.* 
imgui/examples/imgui_impl_glfw.*)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/file_dialog 
${CMAKE_CURRENT_SOURCE_DIR}/libs/gl3w/
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples
)

if(WIN32)
	#At this point, we are using "dirent for Win32"
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dirent/include/)
endif()


add_executable(demo ${demo_src} ./libs/gl3w/GL/gl3w.c)

target_link_libraries(demo glfw OpenGL::GL)

#Set the starter project and the debug directory for visual studio:
if (WIN32)
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT demo)
    set_target_properties(demo PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif ()


