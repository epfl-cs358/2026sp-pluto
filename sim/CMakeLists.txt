cmake_minimum_required(VERSION 3.10)
project(pluto_gait_plugin)

find_package(gz-sim8 REQUIRED)

add_library(pluto_gait_plugin SHARED
  plugins/pluto_gait_plugin.cpp
)

target_link_libraries(pluto_gait_plugin
  gz-sim8::gz-sim8
)