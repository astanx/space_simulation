cmake_minimum_required(VERSION 3.16)
project(Space)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Paths for Homebrew packages
if(APPLE)
    set(GLFW_INCLUDE_DIR "/opt/homebrew/include")
    set(GLFW_LIBRARY "/opt/homebrew/lib/libglfw.dylib")

    # GLEW (Homebrew)
    set(GLEW_INCLUDE_DIR "/opt/homebrew/opt/glew/include")
    set(GLEW_LIBRARY "/opt/homebrew/opt/glew/lib/libGLEW.dylib")
endif()

# Add GLM (header-only)
set(GLM_INCLUDE_DIR "/Users/ivanastanin/vs/cpp/glm-1.0.2")

include_directories(
    ${CMAKE_SOURCE_DIR}/include
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
    src/camera/camera.cpp
    src/graphics/shader.cpp
    src/graphics/texture.cpp
    src/graphics/materials/phongMaterial.cpp
    src/graphics/materials/asteroidMaterial.cpp
    src/graphics/skybox.cpp
    src/graphics/primitives/triangle.cpp
    src/graphics/primitives/primitives.cpp
    src/graphics/primitives/quad.cpp
    src/graphics/primitives/plane.cpp
    src/graphics/primitives/cube.cpp
    src/graphics/primitives/circle.cpp
    src/graphics/primitives/sphere.cpp
    src/graphics/primitives/asteroidShape.cpp
    src/graphics/mesh.cpp
    src/scene/light/light.cpp
    src/scene/light/pointLight.cpp
    src/scene/light/directionalLight.cpp
    src/scene/light/lightManager.cpp
    src/scene/scene.cpp
    src/graphics/model.cpp
    src/resources/OBJloader.cpp
    src/resources/resourceManager.cpp
    src/physics/object.cpp
    src/physics/planet.cpp
    src/physics/constants.cpp
    src/physics/orbit.cpp
    src/physics/moon.cpp
    src/physics/star.cpp
    src/physics/trail.cpp
    src/physics/orbitalObject.cpp
    src/physics/asteroidSystem.cpp
    src/physics/asteroid.cpp
    src/maths/rk4.cpp
    src/maths/random.cpp
)

# Link libraries
target_link_libraries(Space
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARY}
    ${GLEW_LIBRARY}
    "-framework CoreFoundation"
)

# ---- Copy runtime resources (shaders, assets) next to the executable ----
add_custom_command(TARGET Space POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Space>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:Space>/assets
    COMMENT "Copying assets/ to output directory"
)