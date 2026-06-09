cmake_minimum_required(VERSION 3.16)
project(Arugo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/SFML-3.1.0/lib/cmake/SFML")

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(Arugo
    src/main.cpp
    src/Entity.cpp
    src/GameWorld.cpp
    src/InputHandler.cpp
    src/TileMap.cpp
    src/ui/GameOverUI.cpp
    src/battle/commands/BattleCommand.cpp
    src/battle/commands/WaitCommand.cpp
    src/battle/commands/MoveCommand.cpp
    src/battle/ActionQueue.cpp
    src/resources/ResourceManager.cpp
    src/resources/items/Item.cpp
    src/resources/items/ItemRegistry.cpp
    src/ui/TurnBanner.cpp
    src/ui/FlashMessage.cpp
)

target_include_directories(Arugo PRIVATE src)

target_compile_definitions(Arugo PRIVATE SFML_STATIC)

target_link_libraries(Arugo PRIVATE SFML::Graphics SFML::Window SFML::System)

target_link_options(Arugo PRIVATE
    $<$<CONFIG:Release>:-static-libgcc -static-libstdc++>
)

add_custom_command(TARGET Arugo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Arugo>/assets"
    COMMENT "Copy assets..."
)

# --- Distribution (cmake --install build/release --prefix dist) ---
install(TARGETS Arugo DESTINATION .)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION .)
