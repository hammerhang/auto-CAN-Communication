cmake_minimum_required(VERSION 3.21)
project(auto-CAN-Communication)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS regex)
find_package(yaml-cpp REQUIRED)

add_subdirectory(vehicle)

add_executable(auto-CAN-Communication
    vehComm.cpp
)

target_link_libraries(auto-CAN-Communication
    vehicle
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(auto-CAN-Communication vehicle)

