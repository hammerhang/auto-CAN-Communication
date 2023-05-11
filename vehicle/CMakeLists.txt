add_library(vehicle Vehicle.cpp)

target_link_libraries(vehicle PUBLIC
    ${YAML_CPP_LIBRARIES}
    ${Boost_LIBRARIES}
    pthread
)

target_include_directories(vehicle PUBLIC 
    include
    ${CMAKE_CURRENT_SOURCE_DIR}
)
