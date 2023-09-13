cmake_minimum_required(VERSION 3.16)

project(menv)

# Add source files
set(
    SOURCES
    main.cpp
    src/Camera.cpp
    src/Cube.cpp
    src/Shader.cpp
    src/Tokenizer.cpp
    src/Window.cpp
   
)

# Add header files
set(
    HEADERS
    include/core.h
    include/Camera.h
    include/Cube.h
    include/Shader.h
    include/Tokenizer.h
    include/Window.h
)

# Require GL
find_package(OpenGL REQUIRED)

# Add include directories
include_directories(
    include
    imgui
)

# Add library directories
link_directories(
    lib
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} "src/Skeleton.cpp" "src/DOF.cpp" "src/Joint.cpp"   "src/Skin.cpp"  "src/imgui/imgui.cpp" "src/imgui/imgui_demo.cpp" "src/imgui/imgui_draw.cpp" "src/imgui/imgui_impl_glfw.cpp" "src/imgui/imgui_impl_opengl3.cpp" "src/imgui/imgui_tables.cpp" "src/imgui/imgui_widgets.cpp" "include/imgui/imgui_impl_opengl3_loader.h"          "src/Animation.cpp" "src/Channel.cpp" "src/Keyframe.cpp" "src/Particle.cpp" "src/SpringDamper.cpp" "src/Triangle.cpp" "src/Cloth.cpp"  "include/Particle.h" "src/Ground.cpp"     "src/ParticleSystem.cpp" "src/Sphere.cpp" "src/simParticle.cpp" "include/Ground.h" "include/simParticle.h")

# Link libraries
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glew32s.lib glfw3)

# Move assets to .exe
add_custom_target(CopyShaders ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/shaders"
	"${CMAKE_BINARY_DIR}/shaders"
)
add_dependencies(menv CopyShaders)