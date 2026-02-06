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

    set(FREETYPE_INCLUDE_DIR "/opt/homebrew/include/freetype2")
    set(FREETYPE_LIBRARY "/opt/homebrew/lib/libfreetype.dylib")

    # GLEW (Homebrew)
    set(GLEW_INCLUDE_DIR "/opt/homebrew/opt/glew/include")
    set(GLEW_LIBRARY "/opt/homebrew/opt/glew/lib/libGLEW.dylib")
endif()

# Add GLM (header-only)
set(GLM_INCLUDE_DIR "/Users/ivanastanin/vs/cpp/glm-1.0.2")


add_executable(Space 
    # Main
    src/main.cpp

    # Core
    src/core/application.cpp
    src/core/input.cpp

    # Camera
    src/camera/camera.cpp

    # Graphics
    src/graphics/shader.cpp
    src/graphics/texture.cpp
    src/graphics/skybox.cpp
    src/graphics/mesh.cpp
    src/graphics/model.cpp
        #Materials
        src/graphics/materials/phongMaterial.cpp
        src/graphics/materials/asteroidMaterial.cpp
        src/graphics/materials/textMaterial.cpp
    
        # Primitives
        src/graphics/primitives/triangle.cpp
        src/graphics/primitives/primitives.cpp
        src/graphics/primitives/quad.cpp
        src/graphics/primitives/plane.cpp
        src/graphics/primitives/cube.cpp
        src/graphics/primitives/circle.cpp
        src/graphics/primitives/sphere.cpp
        src/graphics/primitives/asteroidShape.cpp
        src/graphics/primitives/ellipsoid.cpp

    # Scene
    src/scene/scene.cpp
        # Light
        src/scene/light/light.cpp
        src/scene/light/pointLight.cpp
        src/scene/light/directionalLight.cpp
        src/scene/light/lightManager.cpp

        # Shadow
        src/scene/shadow/shadow.cpp
        src/scene/shadow/directionalShadow.cpp
        src/scene/shadow/pointShadow.cpp
        src/scene/shadow/shadowManager.cpp

    # Render
    src/render/renderer.cpp
    src/render/textRenderer.cpp
    
    # Resources
    src/resources/OBJloader.cpp
    src/resources/resourceManager.cpp
    src/resources/threadPool.cpp
    
    # Physics
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
    
    # Maths
    src/maths/rk4.cpp
    src/maths/random.cpp
)

include_directories(Space PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${GLFW_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${SOIL2_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(Space PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARY}
    ${GLEW_LIBRARY}
    ${FREETYPE_LIBRARY}
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