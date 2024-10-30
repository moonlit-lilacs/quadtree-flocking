cmake_minimum_required(VERSION 3.23)
project(flocking)

set(CMAKE_CXX_STANDARD 14)

add_executable(flocking
        "quadtree flocking/main.cpp")

set(SFML_ROOT "C:/SFML/SFML-2.6.1")
set(SFML_STATIC_LIBRARIES TRUE)
set(CMAKE_MODULE_PATH "${SFML_ROOT}/cmake/Modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)



# Link SFML libraries
target_link_libraries(flocking sfml-graphics sfml-window sfml-system)
target_include_directories(flocking PRIVATE "${SFML_ROOT}/include")