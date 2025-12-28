cmake_minimum_required(VERSION 3.16)
project(Scape)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Paths for Homebrew packages
if(APPLE)
    set(GLFW_INCLUDE_DIR "/opt/homebrew/include")
    set(GLFW_LIBRARY "/opt/homebrew/lib/libglfw.dylib")
    set(SOIL2_INCLUDE_DIR "/opt/homebrew/include/SOIL2")
    set(SOIL2_LIBRARY "/opt/homebrew/lib/libsoil2-debug.a")

    # GLEW (Homebrew)
    set(GLEW_INCLUDE_DIR "/opt/homebrew/opt/glew/include")
    set(GLEW_LIBRARY "/opt/homebrew/opt/glew/lib/libGLEW.dylib")
endif()

# Add GLM (header-only)
set(GLM_INCLUDE_DIR "/Users/ivanastanin/vs/cpp/glm-1.0.2")

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${GLFW_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${SOIL2_INCLUDE_DIR}
)

link_libraries(
    ${GLFW_LIBRARY}
    ${GLEW_LIBRARY}
    ${OPENGL_LIBRARIES}
)

add_executable(Space 
    src/main.cpp
    src/core/application.cpp
    src/core/camera.cpp
    src/render/shader/shader.cpp
    src/render/texture/texture.cpp
    src/render/material/material.cpp
    src/render/primitives/triangle/triangle.cpp
    src/render/primitives/primitives.cpp
    src/render/primitives/quad/quad.cpp
    src/render/primitives/plane/plane.cpp
    src/render/primitives/cube/cube.cpp
    src/render/mesh/mesh.cpp
    src/render/light/light.cpp
    src/render/model/model.cpp
)

# Link libraries
target_link_libraries(Space
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARY}
    ${GLEW_LIBRARY}
    ${SOIL2_LIBRARY}
    "-framework CoreFoundation"
)

# ---- Copy runtime resources (shaders, assets) next to the executable ----
add_custom_command(TARGET Space POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Space>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:Space>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/textures
            $<TARGET_FILE_DIR:Space>/textures
    COMMENT "Copying assets/ to output directory"
)