cmake_minimum_required(VERSION 3.20)

project(pluto_sim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# MuJoCo is not available via FetchContent, so we require the user to download it manually and specify the path here.
set(MUJOCO_DIR "C:/mujoco")

find_path(MUJOCO_INCLUDE_DIR
    mujoco/mujoco.h
    PATHS
        ${MUJOCO_DIR}/include
    REQUIRED
)

find_library(MUJOCO_LIBRARY
    mujoco
    PATHS
        ${MUJOCO_DIR}/lib
    REQUIRED
)


set(SOURCES
    # Simulation
    src/sim/sim_main.cpp
    src/sim/sim_gait.cpp

    # Shared hardware-independent code
    src/esp/motion/ik_solver.cpp
)

add_executable(pluto_sim ${SOURCES})

target_compile_definitions(pluto_sim PRIVATE
    PLUTO_MODEL_PATH="${CMAKE_SOURCE_DIR}/src/sim/sim_mesh/pluto.xml"
)

target_include_directories(pluto_sim PRIVATE

    # Project root
    ${CMAKE_SOURCE_DIR}/src

    # Simulation
    ${CMAKE_SOURCE_DIR}/src/sim

    # ESP reusable headers
    ${CMAKE_SOURCE_DIR}/src/esp
    ${CMAKE_SOURCE_DIR}/src/esp/motion
    ${CMAKE_SOURCE_DIR}/src/esp/legs

    # MuJoCo
    ${MUJOCO_INCLUDE_DIR}
)

target_link_libraries(pluto_sim PRIVATE
    glfw
    ${MUJOCO_LIBRARY}
)

if (MSVC)

    target_compile_options(pluto_sim PRIVATE
        /W4
    )

    target_compile_definitions(pluto_sim PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )

else()

    target_compile_options(pluto_sim PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()

if (WIN32)
    target_link_libraries(pluto_sim PRIVATE opengl32)
    
    add_custom_command(TARGET pluto_sim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${MUJOCO_DIR}/bin"
            "$<TARGET_FILE_DIR:pluto_sim>"
    )

endif()

set_target_properties(pluto_sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "MuJoCo include dir: ${MUJOCO_INCLUDE_DIR}")
message(STATUS "MuJoCo library: ${MUJOCO_LIBRARY}")