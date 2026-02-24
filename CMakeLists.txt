cmake_minimum_required(VERSION 3.16)
project(Space)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Find OpenGL
find_package(OpenGL REQUIRED)

# GLM (header-only)
find_package(glm QUIET)
if(NOT glm_FOUND)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
    )
    FetchContent_MakeAvailable(glm)
endif()

# GLFW
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
    )
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
endif()

# GLEW
find_package(GLEW QUIET)
if(NOT GLEW_FOUND)
    FetchContent_Declare(
        glew
        GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
        GIT_TAG glew-cmake-2.2.0
    )
    set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glew)
endif()

# FreeType
find_package(Freetype QUIET)
if(NOT Freetype_FOUND)
    FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG VER-2-13-2
    )
    set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(freetype)
endif()

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
        # Materials
        src/graphics/materials/material.cpp
        src/graphics/materials/pbrMaterial.cpp
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
        # State
        src/graphics/state/scopedBlending.cpp
        src/graphics/state/scopedDepthFunc.cpp
        src/graphics/state/scopedDepthMask.cpp
        src/graphics/state/scopedDepthTest.cpp
        src/graphics/state/scopedCullFace.cpp
        src/graphics/state/scopedViewport.cpp
        src/graphics/state/scopedFramebuffer.cpp
        src/graphics/state/scopedVertexArray.cpp
        src/graphics/state/scopedBuffer.cpp
        src/graphics/state/scopedTexture.cpp
        src/graphics/state/scopedShader.cpp
        # Framebuffers
        src/graphics/framebuffers/framebuffer.cpp
        # Buffers
        src/graphics/buffers/buffer.cpp
        src/graphics/buffers/renderBuffer.cpp
        # Arrays
        src/graphics/arrays/vertexArray.cpp
    
    # Scene
    src/scene/scene.cpp
        # Light
        src/scene/light/light.cpp
        src/scene/light/pointLight.cpp
        src/scene/light/phongPointLight.cpp
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
    src/render/postProcess.cpp
    
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
    src/maths/gaussian.cpp
    
    # Debug
    src/debug/logger.cpp
)

target_include_directories(Space PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(Space PRIVATE
    OpenGL::GL
    glm::glm
    glfw
    GLEW::GLEW
    Freetype::Freetype
)

# ---- Copy runtime resources (shaders, assets) next to the executable ----
add_custom_command(TARGET Space POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Space>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Space>/assets
    COMMENT "Copying assets/ to output directory"
)