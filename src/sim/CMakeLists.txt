cmake_minimum_required(VERSION 3.16)
project(pluto_sim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MuJoCo
set(MUJOCO_PATH "/mujoco") # change this to your MuJoCo installation path 

find_path(MUJOCO_INCLUDE_DIR
    mujoco/mujoco.h
    HINTS ${MUJOCO_PATH}/include
)

find_library(MUJOCO_LIB
    mujoco
    HINTS ${MUJOCO_PATH}/lib
)

# GLFW and OpenGL
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Executable
add_executable(pluto_sim
    sim/main.cpp
)

target_include_directories(pluto_sim PRIVATE
    ${MUJOCO_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(pluto_sim
    ${MUJOCO_LIB}
    glfw
    OpenGL::GL
    pthread
    dl
)