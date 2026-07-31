cmake_minimum_required(VERSION 3.20)
project(Minecraft CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. GLFW FetchContent
message(STATUS "[Minecraft CMake] Fetching GLFW 3.4...")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# 2. GLM FetchContent
message(STATUS "[Minecraft CMake] Fetching GLM...")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# 3. EnTT FetchContent
message(STATUS "[Minecraft CMake] Fetching EnTT...")
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.13.2
)
FetchContent_MakeAvailable(entt)

# Source files
set(SOURCES
    src/main.cpp
    src/vendor/glad/glad.c
    src/core/Window.cpp
    src/core/Input.cpp
    src/core/Application.cpp
    src/renderer/Shader.cpp
    src/renderer/Camera.cpp
    src/world/Chunk.cpp
    src/world/ChunkMesh.cpp
    src/world/World.cpp
    src/world/Raycast.cpp
    src/world/SaveSystem.cpp
    src/world/TimeManager.cpp
    src/world/FluidEngine.cpp
    src/world/Biome.cpp
    src/world/RedstoneEngine.cpp
    src/physics/PhysicsEngine.cpp
    src/gui/HUD.cpp
    src/gui/InventoryGUI.cpp
    src/inventory/Inventory.cpp
    src/crafting/CraftingManager.cpp
    src/audio/AudioManager.cpp
)

add_executable(Minecraft ${SOURCES})

target_include_directories(Minecraft PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor
    ${glm_SOURCE_DIR}
    ${entt_SOURCE_DIR}/single_include
)

target_link_libraries(Minecraft PRIVATE
    glfw
    glm::glm
)

if(WIN32)
    target_link_libraries(Minecraft PRIVATE opengl32)
endif()

# Test Engine Target
set(TEST_SOURCES
    src/test_engine.cpp
    src/vendor/glad/glad.c
    src/core/Window.cpp
    src/core/Input.cpp
    src/core/Application.cpp
    src/renderer/Shader.cpp
    src/renderer/Camera.cpp
    src/world/Chunk.cpp
    src/world/ChunkMesh.cpp
    src/world/World.cpp
    src/world/Raycast.cpp
    src/world/SaveSystem.cpp
    src/world/TimeManager.cpp
    src/world/FluidEngine.cpp
    src/world/Biome.cpp
    src/world/RedstoneEngine.cpp
    src/physics/PhysicsEngine.cpp
    src/gui/HUD.cpp
    src/gui/InventoryGUI.cpp
    src/inventory/Inventory.cpp
    src/crafting/CraftingManager.cpp
    src/audio/AudioManager.cpp
)
add_executable(TestEngine ${TEST_SOURCES})
target_include_directories(TestEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor
    ${glm_SOURCE_DIR}
    ${entt_SOURCE_DIR}/single_include
)
target_link_libraries(TestEngine PRIVATE glfw glm::glm)
if(WIN32)
    target_link_libraries(TestEngine PRIVATE opengl32)
endif()

# Copy assets folder to output executable directory
add_custom_command(TARGET Minecraft POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:Minecraft>/assets
    COMMENT "[Minecraft Build] Copying assets to build output directory..."
)
