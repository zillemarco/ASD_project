cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 11)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(ASD_project C CXX)

# Add directory to engine project
add_subdirectory(solver)
add_subdirectory(time_tester)